
ifdef obj
my_inc_base = $(obj)
else
my_inc_base = .
O_TARGET := talitos.o
endif
obj-$(CONFIG_OCF_TALITOS_KTIMER)    += $(obj-base)ktimer/
obj-$(CONFIG_OCF_TALITOS_GTIMER)    += $(obj-base)gtimer/
obj-$(CONFIG_OCF_TALITOS)    += $(obj-base)talitos.o

EXTRA_CFLAGS += -I$(my_inc_base)/.

