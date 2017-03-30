cmd_libbb/makedev.o := /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/bin/powerpc-603-linux-gnu-gcc -Wp,-MD,libbb/.makedev.o.d   -std=gnu99 -Iinclude -Ilibbb  -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D"BB_VER=KBUILD_STR(1.13.3)" -DBB_BT=AUTOCONF_TIMESTAMP  -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wunused -Wunused-parameter -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement -Wold-style-definition -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1 -Os    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(makedev)"  -D"KBUILD_MODNAME=KBUILD_STR(makedev)" -c -o libbb/makedev.o libbb/makedev.c

deps_libbb/makedev.o := \
  libbb/makedev.c \
  include/platform.h \
    $(wildcard include/config/werror.h) \
    $(wildcard include/config///.h) \
    $(wildcard include/config/nommu.h) \
    $(wildcard include/config//nommu.h) \
    $(wildcard include/config//mmu.h) \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/byteswap.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/byteswap.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/endian.h \
    $(wildcard include/config/.h) \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/features.h \
    $(wildcard include/config/c99.h) \
    $(wildcard include/config/ix.h) \
    $(wildcard include/config/ix2.h) \
    $(wildcard include/config/ix199309.h) \
    $(wildcard include/config/ix199506.h) \
    $(wildcard include/config/en.h) \
    $(wildcard include/config/en/extended.h) \
    $(wildcard include/config/x98.h) \
    $(wildcard include/config/en2k.h) \
    $(wildcard include/config/gefile.h) \
    $(wildcard include/config/gefile64.h) \
    $(wildcard include/config/e/offset64.h) \
    $(wildcard include/config/d.h) \
    $(wildcard include/config/c.h) \
    $(wildcard include/config/ntrant.h) \
    $(wildcard include/config/tify/level.h) \
    $(wildcard include/config/i.h) \
    $(wildcard include/config/ern/inlines.h) \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/cdefs.h \
    $(wildcard include/config/espaces.h) \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/gnu/stubs.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/endian.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/arpa/inet.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/netinet/in.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/stdint.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/wchar.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/wordsize.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/socket.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/uio.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/types.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/types.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/include/stddef.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/typesizes.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/time.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/select.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/select.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/sigset.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/time.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/sysmacros.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/pthreadtypes.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/sched.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/uio.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/socket.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/include/limits.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/include/syslimits.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/limits.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/posix1_lim.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/local_lim.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/linux/limits.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/posix2_lim.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/xopen_lim.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/stdio_lim.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/sockaddr.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/asm/socket.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/in.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/include/stdbool.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/mount.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/ioctl.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/ioctls.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/asm/ioctls.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/asm/ioctl.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/ioctl-types.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/termios.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/termios.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/ttydefaults.h \

libbb/makedev.o: $(deps_libbb/makedev.o)

$(deps_libbb/makedev.o):
