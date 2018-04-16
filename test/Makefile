%.o: %.c
	arm-linux-gnueabi-gcc -c -I../include -o $@ $<

all: trial policy tester selector

trial: trial.o
	arm-linux-gnueabi-gcc -I../include -o $@ $<

policy: policy.o
	arm-linux-gnueabi-gcc -I../include -o $@ $<

selector: selector.o
	arm-linux-gnueabi-gcc -I../include -o $@ $<

tester: tester.o
	arm-linux-gnueabi-gcc -I../include -o $@ $<

clean: 
	rm -rf *.o 
	rm -rf trial policy selector tester
