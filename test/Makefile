%.o: %.c
	arm-linux-gnueabi-gcc -c -I../include -o $@ $<

all: selector.o
	arm-linux-gnueabi-gcc -I../include -o selector selector.o
