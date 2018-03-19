%.o: %.c
	arm-linux-gnueabi-gcc -c -I../include -o $@ $<

all: test_ptree.o
	arm-linux-gnueabi-gcc -I../include -o test_ptree test_ptree.o
