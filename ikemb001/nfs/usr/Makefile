CC=/usr/local/arm/3.4.1/bin/arm-linux-gcc
CFLAGS= -Wall -I /usr/local/arm/3.4.1/arm-linux/include -lminigui -lpthread \
 -I /root/arm/minigui/include -L /root/arm/minigui/lib

hello:hello.c
	$(CC) $(CFLAGS) -o $@ $<
clean:
	rm -f *.o
	rm -f mini
