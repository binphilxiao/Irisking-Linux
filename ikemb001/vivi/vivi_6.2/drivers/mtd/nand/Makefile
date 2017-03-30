#
# linux/drivers/nand/Makefile
#
# $Id: Makefile,v 1.1.1.1 2004/02/04 10:37:37 laputa Exp $

O_TARGET	:= nandlink.o


obj-y			:=
obj-$(CONFIG_MTD_NAND_ECC)	+= nand_ecc.o
obj-$(CONFIG_MTD_SMC)		+= smc_core.o bon.o
obj-$(CONFIG_MTD_BONFS)		+= bon.o

include $(TOPDIR)/Rules.make
