CC=/usr/local/arm/3.4.1/bin/arm-linux-gcc
CFLAGS= -Wall -I /usr/local/arm/3.4.1/arm-linux/include

loaddsp:loaddsp.c
	$(CC) $(CFLAGS) -o $@ $<
clean:
	rm -f *.o
	rm -f loaddsp
