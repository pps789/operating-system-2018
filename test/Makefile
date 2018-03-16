%.o: %.c
	arm-linux-gnueabi-gcc -c -I../include -o $@ $<

all: test.o
	arm-linux-gnueabi-gcc -I../include -o test test.o
