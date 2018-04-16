%.o: %.c
	arm-linux-gnueabi-gcc -c -I../include -o $@ $<

all: trial.o
	arm-linux-gnueabi-gcc -I../include -o trial trial.o

clean: 
	rm -rf *.o 
