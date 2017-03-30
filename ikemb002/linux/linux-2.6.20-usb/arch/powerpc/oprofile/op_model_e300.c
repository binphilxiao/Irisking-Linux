/*
 * oprofile/op_model_e300.c
 *
 * Freescale E300 oprofile support, based on ppc64 oprofile support
 * Copyright (C) 2004 Anton Blanchard <anton@au.ibm.com>, IBM
 *
 * Copyright (c) 2004 Freescale Semiconductor, Inc
 *
 * Author: Harninder Rai
 * Maintainer: Kumar Gala <galak@kernel.crashing.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/oprofile.h>
#include <linux/init.h>
#include <linux/smp.h>
#include <asm/ptrace.h>
#include <asm/system.h>
#include <asm/processor.h>
#include <asm/cputable.h>
#include <asm/reg_booke.h>
#include <asm/page.h>
/* Temporary hack to circumvent warnings */
#ifndef CONFIG_FSL_BOOKE
#define CONFIG_FSL_BOOKE
#include <asm/pmc.h>
#include <asm/oprofile_impl.h>
#undef CONFIG_FSL_BOOKE
#else
#include <asm/pmc.h>
#endif

#define	MAX_COUNTER_VAL	0x80000000UL

static unsigned long reset_value[OP_MAX_COUNTER];

static int num_counters;
static int oprofile_running;

void init_pmc_stop(int ctr)
{
	u32 pmlca = (PMLCA_FC | PMLCA_FCS | PMLCA_FCU |
		     PMLCA_FCM1 | PMLCA_FCM0);
	u32 pmlcb = 0;

	switch (ctr) {
	case 0:
		mtpmr(PMRN_PMLCA0, pmlca);
		mtpmr(PMRN_PMLCB0, pmlcb);
		break;
	case 1:
		mtpmr(PMRN_PMLCA1, pmlca);
		mtpmr(PMRN_PMLCB1, pmlcb);
		break;
	case 2:
		mtpmr(PMRN_PMLCA2, pmlca);
		mtpmr(PMRN_PMLCB2, pmlcb);
		break;
	case 3:
		mtpmr(PMRN_PMLCA3, pmlca);
		mtpmr(PMRN_PMLCB3, pmlcb);
		break;
	default:
		panic("Bad ctr number!\n");
	}
}

void set_pmc_event(int ctr, int event)
{
	u32 pmlca;

	pmlca = get_pmlca(ctr);

	pmlca = (pmlca & ~PMLCA_EVENT_MASK) |
	    ((event << PMLCA_EVENT_SHIFT) & PMLCA_EVENT_MASK);

	set_pmlca(ctr, pmlca);
}

void set_pmc_user_kernel(int ctr, int user, int kernel)
{
	u32 pmlca;

	pmlca = get_pmlca(ctr);

	if (user)
		pmlca &= ~PMLCA_FCU;
	else
		pmlca |= PMLCA_FCU;

	if (kernel)
		pmlca &= ~PMLCA_FCS;
	else
		pmlca |= PMLCA_FCS;

	set_pmlca(ctr, pmlca);
}

void set_pmc_marked(int ctr, int mark0, int mark1)
{
	u32 pmlca = get_pmlca(ctr);

	if (mark0)
		pmlca &= ~PMLCA_FCM0;
	else
		pmlca |= PMLCA_FCM0;

	if (mark1)
		pmlca &= ~PMLCA_FCM1;
	else
		pmlca |= PMLCA_FCM1;

	set_pmlca(ctr, pmlca);
}

void pmc_start_ctr(int ctr, int enable)
{
	u32 pmlca = get_pmlca(ctr);

	pmlca &= ~PMLCA_FC;

	if (enable)
		pmlca |= PMLCA_CE;
	else
		pmlca &= ~PMLCA_CE;

	set_pmlca(ctr, pmlca);
}

void pmc_start_ctrs(int enable)
{
	u32 pmgc0 = mfpmr(PMRN_PMGC0);

	pmgc0 &= ~PMGC0_FAC;
	pmgc0 |= PMGC0_FCECE;

	if (enable)
		pmgc0 |= PMGC0_PMIE;
	else
		pmgc0 &= ~PMGC0_PMIE;

	mtpmr(PMRN_PMGC0, pmgc0);
}

void pmc_stop_ctrs(void)
{
	u32 pmgc0 = mfpmr(PMRN_PMGC0);

	pmgc0 |= PMGC0_FAC;

	pmgc0 &= ~(PMGC0_PMIE | PMGC0_FCECE);

	mtpmr(PMRN_PMGC0, pmgc0);
}

static void e300_reg_setup(struct op_counter_config *ctr,
			   struct op_system_config *sys, int num_ctrs)
{
	int i;

	num_counters = num_ctrs;

	/* freeze all counters */
	pmc_stop_ctrs();

	/* 
	 * Our counters count up, and "count" refers to
	 * how much before the next interrupt, and we interrupt
	 * on overflow.  So we calculate the starting value
	 * which will give us "count" until overflow.
	 * Then we set the events on the enabled counters 
	 */
	for (i = 0; i < num_counters; i++) {
		reset_value[i] = MAX_COUNTER_VAL - ctr[i].count;

		init_pmc_stop(i);

		set_pmc_event(i, ctr[i].event);

		set_pmc_user_kernel(i, ctr[i].user, ctr[i].kernel);
	}
}

static void e300_start(struct op_counter_config *ctr)
{
	int i;

	mtmsr(mfmsr() | MSR_PMM);

	for (i = 0; i < num_counters; i++) {
		if (ctr[i].enabled) {
			ctr_write(i, reset_value[i]);
			/* 
			 * Set Each enabled counter to only 
			 * count when the Mark bit is not set 
			 */
			set_pmc_marked(i, 1, 0);
			pmc_start_ctr(i, 1);
		} else {
			ctr_write(i, 0);
			pmc_start_ctr(i, 0);
		}
	}

	/* 
	 * Clear the freeze bit, and enable the interrupt.
	 * The counters won't actually start until the rfi clears
	 * the PMM bit 
	 */
	pmc_start_ctrs(1);

	oprofile_running = 1;

	printk("start on cpu %d, pmgc0 %x\n", smp_processor_id(),
	       mfpmr(PMRN_PMGC0));
}

static void e300_stop(void)
{
	pmc_stop_ctrs();

	oprofile_running = 0;

	printk("stop on cpu %d, pmgc0 %x\n", smp_processor_id(),
	       mfpmr(PMRN_PMGC0));

	mb();
}

static void e300_handle_interrupt(struct pt_regs *regs,
				  struct op_counter_config *ctr)
{
	unsigned long pc;
	int is_kernel;
	int val;
	int i;

	/* set the PMM bit (see comment below) */
	mtmsr(mfmsr() | MSR_PMM);

	pc = regs->nip;
	is_kernel = is_kernel_addr(pc);

	for (i = 0; i < num_counters; i++) {
		val = ctr_read(i);
		if (val < 0) {
			if (oprofile_running && ctr[i].enabled) {
				oprofile_add_ext_sample(pc, regs, i, is_kernel);
				ctr_write(i, reset_value[i]);
			} else {
				ctr_write(i, 0);
			}
		}
	}

	/* 
	 * The freeze bit was set by the interrupt. 
	 * Clear the freeze bit, and reenable the interrupt.
	 * The counters won't actually start until the rfi clears
	 * the PMM bit 
	 */
	pmc_start_ctrs(1);
}

static void e300_cpu_setup(void *a)
{
	/* Do nothing */
	return;
}

struct op_powerpc_model op_model_e300 = {
	.reg_setup = e300_reg_setup,
	.start = e300_start,
	.cpu_setup = e300_cpu_setup,
	.stop = e300_stop,
	.handle_interrupt = e300_handle_interrupt,
};
