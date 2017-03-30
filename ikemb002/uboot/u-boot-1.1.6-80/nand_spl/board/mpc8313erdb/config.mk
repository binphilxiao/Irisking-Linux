#
# (C) Copyright 2007, Freescale Semiconductor Inc.
#
# (C) Copyright 2006
# Stefan Roese, DENX Software Engineering, sr@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#
#
# Freescale MPC8313ERDB Reference Platform board
#
#
# TEXT_BASE for SPL:
#
# On Boot-from-NAND the FCM buffer is mapped to the boot space. This
# will be either 0x00000000 for low boot or 0xFFF00000 for high boot.
# The u-boot NAND bootload is configured to use high boot only so
# that it does not conflict with the DDR memory which is configured
# at 0x00000000. Accordingly TEXT_BASE is set to the high boot memory
# space at 0xFFF00000.
#
TEXT_BASE = 0xFFF00000

# PAD_TO used to generate a 16kByte binary needed for the combined image
# -> PAD_TO = TEXT_BASE + 0x4000
PAD_TO	= 0xFFF04000

ifeq ($(debug),1)
PLATFORM_CPPFLAGS += -DDEBUG
endif

