%.o: %.c
	arm-linux-gnueabi-gcc -c -I../include -o $@ $<
all : selector trial tester

selector : selector.o
	arm-linux-gnueabi-gcc -I../include -o selector selector.o

trial: trial.o
	arm-linux-gnueabi-gcc -I../include -o trial trial.o

tester: tester.o
	arm-linux-gnueabi-gcc -I../include -o tester tester.o

clean: 
	rm -rf *.o 
