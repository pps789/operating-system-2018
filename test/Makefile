%.o: %.c
	arm-linux-gnueabi-gcc -c -I../include -o $@ $<

all: gpsupdate.o
	arm-linux-gnueabi-gcc -I../include -o gpsupdate gpsupdate.o
