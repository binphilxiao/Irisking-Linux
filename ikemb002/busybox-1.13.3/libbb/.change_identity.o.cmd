cmd_libbb/change_identity.o := /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/bin/powerpc-603-linux-gnu-gcc -Wp,-MD,libbb/.change_identity.o.d   -std=gnu99 -Iinclude -Ilibbb  -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D"BB_VER=KBUILD_STR(1.13.3)" -DBB_BT=AUTOCONF_TIMESTAMP  -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wunused -Wunused-parameter -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement -Wold-style-definition -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1 -Os    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(change_identity)"  -D"KBUILD_MODNAME=KBUILD_STR(change_identity)" -c -o libbb/change_identity.o libbb/change_identity.c

deps_libbb/change_identity.o := \
  libbb/change_identity.c \
  include/libbb.h \
    $(wildcard include/config/selinux.h) \
    $(wildcard include/config/locale/support.h) \
    $(wildcard include/config/feature/shadowpasswds.h) \
    $(wildcard include/config/use/bb/pwd/grp.h) \
    $(wildcard include/config/use/bb/shadow.h) \
    $(wildcard include/config/lfs.h) \
    $(wildcard include/config/feature/buffers/go/on/stack.h) \
    $(wildcard include/config/buffer.h) \
    $(wildcard include/config/ubuffer.h) \
    $(wildcard include/config/feature/buffers/go/in/bss.h) \
    $(wildcard include/config/inux.h) \
    $(wildcard include/config/feature/ipv6.h) \
    $(wildcard include/config/feature/check/names.h) \
    $(wildcard include/config/feature/prefer/applets.h) \
    $(wildcard include/config/busybox/exec/path.h) \
    $(wildcard include/config/getopt/long.h) \
    $(wildcard include/config/feature/pidfile.h) \
    $(wildcard include/config/feature/syslog.h) \
    $(wildcard include/config/feature/individual.h) \
    $(wildcard include/config/o.h) \
    $(wildcard include/config/ntf.h) \
    $(wildcard include/config/t.h) \
    $(wildcard include/config/l.h) \
    $(wildcard include/config/wn.h) \
    $(wildcard include/config/.h) \
    $(wildcard include/config/route.h) \
    $(wildcard include/config/feature/hwib.h) \
    $(wildcard include/config/feature/crond/d.h) \
    $(wildcard include/config/use/bb/crypt.h) \
    $(wildcard include/config/ioctl/hex2str/error.h) \
    $(wildcard include/config/feature/editing.h) \
    $(wildcard include/config/feature/editing/history.h) \
    $(wildcard include/config/ture/editing/savehistory.h) \
    $(wildcard include/config/feature/editing/savehistory.h) \
    $(wildcard include/config/feature/tab/completion.h) \
    $(wildcard include/config/feature/username/completion.h) \
    $(wildcard include/config/feature/editing/vi.h) \
    $(wildcard include/config/feature/topmem.h) \
    $(wildcard include/config/feature/top/smp/process.h) \
    $(wildcard include/config/killall.h) \
    $(wildcard include/config/pgrep.h) \
    $(wildcard include/config/pkill.h) \
    $(wildcard include/config/pidof.h) \
    $(wildcard include/config/sestatus.h) \
    $(wildcard include/config/feature/devfs.h) \
  include/platform.h \
    $(wildcard include/config/werror.h) \
    $(wildcard include/config///.h) \
    $(wildcard include/config/nommu.h) \
    $(wildcard include/config//nommu.h) \
    $(wildcard include/config//mmu.h) \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/byteswap.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/byteswap.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/endian.h \
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
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/ctype.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/xlocale.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/dirent.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/dirent.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/errno.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/errno.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/linux/errno.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/asm/errno.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/fcntl.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/fcntl.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/stat.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/stat.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/inttypes.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/netdb.h \
    $(wildcard include/config/3/ascii/rules.h) \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/rpc/netdb.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/siginfo.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/netdb.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/setjmp.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/setjmp.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/signal.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/signum.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/sigaction.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/sigcontext.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/asm/sigcontext.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/asm/ptrace.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/sigstack.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/ucontext.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/sigthread.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/stdio.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/libio.h \
    $(wildcard include/config/a.h) \
    $(wildcard include/config/ar/t.h) \
    $(wildcard include/config//io/file.h) \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/_G_config.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/wchar.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/gconv.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/include/stdarg.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/sys_errlist.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/stdlib.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/waitflags.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/waitstatus.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/alloca.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/string.h \
    $(wildcard include/config/ing/inlines.h) \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/poll.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/poll.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/mman.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/mman.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/time.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/wait.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/resource.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/resource.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/unistd.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/posix_opt.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/environments.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/confname.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/getopt.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/utime.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/param.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/linux/param.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/asm/param.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/mntent.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/paths.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/sys/statfs.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/statfs.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/locale.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/bits/locale.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/pwd.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/grp.h \
  /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/lib/gcc/powerpc-603-linux-gnu/4.1.0/../../../../powerpc-603-linux-gnu/sys-include/shadow.h \
  include/pwd_.h \
  include/grp_.h \
  include/shadow_.h \
  include/xatonum.h \

libbb/change_identity.o: $(deps_libbb/change_identity.o)

$(deps_libbb/change_identity.o):
