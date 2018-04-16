%.o: %.c
	arm-linux-gnueabi-gcc -c -I../include -o $@ $<

all: policy.o
	arm-linux-gnueabi-gcc -I../include -o policy policy.o

clean: 
	rm -rf *.o 
