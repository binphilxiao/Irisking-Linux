cmd_sysklogd/lib.a := rm -f sysklogd/lib.a; /opt/crosstool/gcc-4.1.0-glibc-2.3.6/powerpc-603-linux-gnu/bin/powerpc-603-linux-gnu-ar  rcs sysklogd/lib.a sysklogd/klogd.o sysklogd/logread.o sysklogd/syslogd_and_logger.o