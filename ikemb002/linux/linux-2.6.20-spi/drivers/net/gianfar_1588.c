/*
 * drivers/net/gianfar_1588.c
 *
 * Gianfar Ethernet Driver -- IEEE 1588 interface functionality
 *
 * Copyright (c) 2007 Freescale Semiconductor, Inc.
 *
 * Author: Anup Gangwar <anup.gangwar@freescale.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <asm/ocp.h>

#include <linux/io.h>
#include <linux/irq.h>
#include <linux/uaccess.h>

#include "gianfar.h"

static int gfar_ptp_init_circ(struct gfar_ptp_circular_t *buf);
static int gfar_ptp_is_empty(struct gfar_ptp_circular_t *buf);
static int gfar_ptp_nelems(struct gfar_ptp_circular_t *buf);
static int gfar_ptp_is_full(struct gfar_ptp_circular_t *buf);
static int gfar_ptp_insert(struct gfar_ptp_circular_t *buf,
			struct gfar_ptp_data_t *data);
static int gfar_ptp_find_and_remove(struct gfar_ptp_circular_t *buf,
			int key, struct gfar_ptp_data_t *data);

/*
 * Resource required for accessing 1588 Timer Registers. There are few 1588
 * modules registers which are present in eTSEC1 memory space only. The second
 * reg entry there in denotes the 1588 regs.
 */
int gfar_ptp_init(struct gfar_private *priv,
		struct gianfar_platform_data *einfo)
{
	priv->regs_1588 = ioremap(einfo->regs_1588, sizeof (struct gfar));
	if ((priv->regs_1588 == NULL) ||
			gfar_ptp_init_circ(&(priv->rx_time_sync)) ||
			gfar_ptp_init_circ(&(priv->rx_time_del_req))) {
		return 1;
	}

	return 0;
}

void gfar_ptp_cleanup(struct gfar_private *priv)
{
	if (priv->regs_1588 != NULL)
		iounmap(priv->regs_1588);
	if (priv->rx_time_sync.data_buf)
		vfree(priv->rx_time_sync.data_buf);
	if (priv->rx_time_del_req.data_buf)
		vfree(priv->rx_time_del_req.data_buf);
}

int gfar_ptp_do_txstamp(struct sk_buff *skb)
{
	u32 *multicast_addr;
	char *pkt_type;

	if (skb->len > 42) {
		pkt_type = (char *)(skb->data + 31);
		multicast_addr = (u32 *)(skb->data + 38);

		if ((*multicast_addr == 0xe0000181) && (*pkt_type == 0x11))
			return 1;
	}

	return 0;
}

void gfar_ptp_store_rxstamp(struct net_device *dev, struct sk_buff *skb)
{
	int is_not_igmp, msg_type, seq_id;
	struct gfar_ptp_data_t tmp_rx_time;
	struct gfar_private *priv = netdev_priv(dev);

	is_not_igmp = (*((char *)skb->data) + 32) != 0x2;
	seq_id = *((u16 *)(skb->data + 80));
	msg_type = *((u8 *)(skb->data + 82));

	if (is_not_igmp && (seq_id != 0)) {
		tmp_rx_time.key = seq_id;
		tmp_rx_time.item.high = *((u32 *)skb->data);
		tmp_rx_time.item.low = *(((u32 *)skb->data) + 1);

		switch (msg_type) {
		case GFAR_PTP_MSG_TYPE_SYNC: 
			gfar_ptp_insert(&(priv->rx_time_sync), &tmp_rx_time);
			break;
		case GFAR_PTP_MSG_TYPE_DEL_REQ: 
			gfar_ptp_insert(&(priv->rx_time_del_req), &tmp_rx_time);
			break;
		default:
			break;
		}
	}
}

int gfar_ptp_init_circ(struct gfar_ptp_circular_t *buf)
{
	buf->data_buf = (struct gfar_ptp_data_t *)
				vmalloc((DEFAULT_PTP_RX_BUF_SZ+1) *
					sizeof(struct gfar_ptp_data_t));

	if (!buf->data_buf)
		return 1;
	buf->front = buf->end = 0;
	buf->size = (DEFAULT_PTP_RX_BUF_SZ + 1);

	return 0;
}

static inline int gfar_ptp_calc_index (int size, int curr_index, int offset)
{
	return ((curr_index + offset) % size);
}

int gfar_ptp_is_empty (struct gfar_ptp_circular_t *buf)
{
	return (buf->front == buf->end);
}

int gfar_ptp_nelems (struct gfar_ptp_circular_t *buf)
{
	const int front = buf->front;
	const int end = buf->end;
	const int size = buf->size;
	int n_items;

	if (end > front)
		n_items = end - front;
	else if (end < front)
		n_items = size - (front - end);
	else
		n_items = 0;

	return n_items;
}

int gfar_ptp_is_full (struct gfar_ptp_circular_t *buf)
{
	if (gfar_ptp_nelems(buf) == (buf->size - 1))
		return 1;
	else
		return 0;
}

int gfar_ptp_insert (struct gfar_ptp_circular_t *buf,
				struct gfar_ptp_data_t *data)
{
	struct gfar_ptp_data_t *tmp;

	if (gfar_ptp_is_full(buf))
		return 1;

	tmp = (buf->data_buf + buf->end);

	tmp->key = data->key;
	tmp->item.high = data->item.high;
	tmp->item.low = data->item.low;

	buf->end = gfar_ptp_calc_index(buf->size, buf->end, 1);

	return 0;
}

int gfar_ptp_find_and_remove (struct gfar_ptp_circular_t *buf,
			int key, struct gfar_ptp_data_t *data)
{
	int i;
	int size = buf->size, end = buf->end;

	if (gfar_ptp_is_empty(buf))
		return 1;

	i = buf->front;
	while (i != end) {
		if ((buf->data_buf + i)->key == key)
			break;
		i = gfar_ptp_calc_index(size, i, 1);
	}

	if (i == end) {
		buf->front = buf->end;
		return 1;
	}

	data->item.high = (buf->data_buf + i)->item.high;
	data->item.low = (buf->data_buf + i)->item.low;

	buf->front = gfar_ptp_calc_index(size, i, 1);

	return 0;
}

static void gfar_adj_freq(signed long freq)
{
	char new_v, old_v;
	const char max_v = TMR_MAX_DAC_V;
	const char center_v = max_v / 2;
	const long max_freq = TMR_PTPD_MAX_FREQ;

	old_v = ad5301_get_voltage();
	/* Linear interpolation for our DAC range */
	new_v = (freq * center_v) / max_freq + center_v;

	ad5301_set_voltage(new_v);
}

/* Set the 1588 timer counter registers */
static void gfar_set_1588cnt(struct net_device *dev,
			struct gfar_ptp_time *gfar_time)
{
	struct gfar_private *priv = netdev_priv(dev);
	u32 tempval;

	/* We must write the tmr_cnt_l register first */
	tempval = (u32)gfar_time->low;
	gfar_write(&(priv->regs_1588->tmr_cnt_l), tempval);
	tempval = (u32)gfar_time->high;
	gfar_write(&(priv->regs_1588->tmr_cnt_h), tempval);
}

/* Get both the time-stamps and use the larger one */
static void gfar_get_tx_timestamp(struct gfar __iomem *regs,
			struct gfar_ptp_time *tx_time)
{
	struct gfar_ptp_time tx_set_1, tx_set_2;
	u32 tmp;

	/* Read the low register first */
	tx_set_1.low = gfar_read(&regs->tmr_txts1_l);
	tx_set_1.high = gfar_read(&regs->tmr_txts1_h);

	tx_set_2.low = gfar_read(&regs->tmr_txts2_l);
	tx_set_2.high = gfar_read(&regs->tmr_txts2_h);

	tmp = 0;
	if ( tx_set_2.high > tx_set_1.high )
		tmp = 1;
	else if ( tx_set_2.high == tx_set_1.high )
		if ( tx_set_2.low > tx_set_1.low )
			tmp = 1;

	if (tmp == 0) {
		tx_time->low = tx_set_1.low;
		tx_time->high = tx_set_1.high;
	} else {
		tx_time->low = tx_set_2.low;
		tx_time->high = tx_set_2.high;
	}
}

static void gfar_get_rx_time(struct gfar_private *priv, struct ifreq *ifr,
		struct gfar_ptp_time *rx_time, int mode)
{
	struct gfar_ptp_data_t tmp;
	int key, flag;

	key = *((int *)ifr->ifr_data);

	switch (mode) {
	case PTP_GET_RX_TIMESTAMP_SYNC:
		flag = gfar_ptp_find_and_remove(&(priv->rx_time_sync),
						key, &tmp);
		break;
	case PTP_GET_RX_TIMESTAMP_DEL_REQ:
		flag = gfar_ptp_find_and_remove(&(priv->rx_time_del_req),
						key, &tmp);
		break;
	default:
		flag = 1;
		printk(KERN_ERR "ERROR\n");
		break;
	}

	if (!flag) {
		rx_time->high = tmp.item.high;
		rx_time->low = tmp.item.low;
	} else {
		rx_time->high = rx_time->low = 0;
	}
}

static void gfar_get_curr_cnt(struct gfar __iomem *regs_1588,
			struct gfar_ptp_time *curr_time)
{
	curr_time->low = gfar_read(&(regs_1588->tmr_cnt_l));
	curr_time->high = gfar_read(&(regs_1588->tmr_cnt_h));
}

static void gfar_adj_addend(struct gfar __iomem *regs_1588, signed long adj)
{
	unsigned long new;
	const long mask = (TMR_PTPD_MAX_FREQ - 1);
	const long mid = mask - (TMR_ADD_VAL & mask);
	const long half_max = TMR_PTPD_MAX_FREQ;
	signed long tmp;

	/* Do a linear interpolation */
	tmp = (((long long)adj) * ((long long)mid)) / ((long long)half_max);
	new = ((unsigned long)TMR_ADD_VAL) + tmp;

	gfar_write(&(regs_1588->tmr_add), new);
}

/* IOCTL Handler for PTP Specific IOCTL Commands coming from PTPD Application */
int gfar_1588_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct gfar_private *priv = netdev_priv(dev);
	struct gfar __iomem *regs = priv->regs;
	struct gfar_ptp_time *cnt;
	signed long *freq, *p_addend, addend;
	struct gfar_ptp_time rx_time, tx_time, curr_time;

	switch (cmd) {
	case PTP_GET_RX_TIMESTAMP_SYNC:
	case PTP_GET_RX_TIMESTAMP_DEL_REQ:
		gfar_get_rx_time(priv, ifr, &rx_time, cmd);
		copy_to_user(ifr->ifr_data, &rx_time, sizeof(rx_time));
		break;
	case PTP_GET_TX_TIMESTAMP:
		gfar_get_tx_timestamp(regs, &tx_time);
		copy_to_user(ifr->ifr_data, &tx_time, sizeof(tx_time));
		break;
	case PTP_GET_CNT:
		gfar_get_curr_cnt(priv->regs_1588, &curr_time);
		copy_to_user(ifr->ifr_data, &curr_time, sizeof(curr_time));
		break;
	case PTP_SET_CNT:
		cnt = (struct gfar_ptp_time *)ifr->ifr_data;
		gfar_set_1588cnt(dev, cnt);
		break;
	case PTP_ADJ_FREQ:
		freq = (signed long *)ifr->ifr_data;
		gfar_adj_freq(*freq);
		break;
	case PTP_ADJ_ADDEND:
		p_addend = (signed long *)ifr->ifr_data;
		gfar_adj_addend(priv->regs_1588, *p_addend);
		break;
	case PTP_ADJ_ADDEND_IXXAT:
		p_addend = (signed long *)ifr->ifr_data;
		/* assign new value directly */
		gfar_write(&(priv->regs_1588->tmr_add), *p_addend);
		break;
	case PTP_GET_ADDEND:
		addend = TMR_ADD_VAL;
		/* return initial timer add value
		 * to calculate drift correction */
		copy_to_user(ifr->ifr_data, &addend, sizeof(addend));
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

/* Function to initialize Filer Entry
 * far: Offset in Filer Table in SRAM
 * fcr: Filer Control register value corresponds to table entry
 * fpr: Filer Entry Value
 */
inline void gfar_write_filer(struct net_device *dev, unsigned int far,
			unsigned int fcr, unsigned int fpr)
{
	struct gfar_private *priv = netdev_priv(dev);
	gfar_write(&priv->regs->rqfar, far);
	gfar_write(&priv->regs->rqfcr, fcr);
	gfar_write(&priv->regs->rqfpr, fpr);
}

/* 1588 Module intialization and filer table populating routine*/
void gfar_1588_start(struct net_device *dev)
{
	struct gfar_private *priv = netdev_priv(dev);

	/* Arbitrary Field register */
	gfar_write(&priv->regs->rbifx, 0xE8);

	/* File on port number 13f to 141 */
	gfar_write_filer(dev, 0x0, 0x80, 0xffff);
	gfar_write_filer(dev, 0x1, 0xaf, 0x13f);
	gfar_write_filer(dev, 0x2, 0xef, 0x141);
	/* File if type field is ipv4 packet */
	gfar_write_filer(dev, 0x3, 0x87, 0x800);

	/* File for mulitcast address in range from
	 * 224.0.1.129 to 224.0.1.132 */
	gfar_write_filer(dev, 0x4, 0x80, 0xffffffff);
	gfar_write_filer(dev, 0x5, 0xac, 0xe0000181);
	gfar_write_filer(dev, 0x6, 0xec, 0xe0000185);

	/* File is protocol field in UDP */
	gfar_write_filer(dev, 0x7, 0x80, 0xff);
	gfar_write_filer(dev, 0x8, 0x8b, 0x11);

	/* Check for PTP Packet type being less than 5 i.e. only SYNC,
	 * Delay_Req, Follow-up, Delay-res, Management Packet types are
	 * filed with this rule
	 */
	gfar_write_filer(dev, 0x9, 0x62, 0x5);
	gfar_write_filer(dev, 0xa, 0x80, 0xffff);
	gfar_write_filer(dev, 0xb, 0x87, 0x800);

	/* Accept IGMP query messages as well */
	gfar_write_filer(dev, 0xc, 0x80, 0xffffffff);
	gfar_write_filer(dev, 0xd, 0xc, 0xe0000001);
	/* Default Rule to file all non-PTP packets in Queue 1*/
	gfar_write_filer(dev, 0xe, 0x420, 0x0);

#if defined(CONFIG_GFAR_PTP_VCO)
	/* Select 1588 Timer source and enable module for starting Tmr Clock */
	gfar_write(&(priv->regs_1588->tmr_ctrl),
		gfar_read(&(priv->regs_1588->tmr_ctrl)) |
		TMR_CTRL_ENABLE | TMR_CTRL_EXT_CLK );
#else
	/* Select 1588 Timer source and enable module for starting Tmr Clock */
	gfar_write(&(priv->regs_1588->tmr_ctrl),
		gfar_read(&(priv->regs_1588->tmr_ctrl)) |
		TMR_CTRL_ENABLE | TMR_CTRL_SYS_CLK );
#endif
	/* Need to mask the TCLK bits as they are initialized with 1 */
	gfar_write(&(priv->regs_1588->tmr_ctrl),
		(gfar_read(&(priv->regs_1588->tmr_ctrl))
		 & ~TMR_CTRL_TCLK_MASK) | TMR_CTRL_TCLK_PRD );

	/* Nominal Freq = 10MHz */
	gfar_write(&(priv->regs_1588->tmr_add), TMR_ADD_VAL);
}

/* Cleanup routine for 1588 module.
 * When PTP is disabled this routing is called */
void gfar_1588_stop(struct net_device *dev)
{
	struct gfar_private *priv = netdev_priv(dev);
	unsigned int filer_addr_reg;

	for (filer_addr_reg = 0; filer_addr_reg < 0xe; filer_addr_reg++) {
		gfar_write_filer(dev, filer_addr_reg, 0, 0);
	}

	gfar_write(&(priv->regs_1588->tmr_ctrl),
		gfar_read(&(priv->regs_1588->tmr_ctrl))
		& ~TMR_CTRL_ENABLE);
}
