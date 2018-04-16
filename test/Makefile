OBJ = selector trial rotd
TESTER = rlocker wlocker runlocker wunlocker
%.o: %.c
	arm-linux-gnueabi-gcc -c -I../include -o $@ $<
all : $(OBJ)

test : $(TESTER)

rotd : rotd.o
	arm-linux-guneabi-gcc -I../include -o rotd rotd.o

selector : selector.o
	arm-linux-gnueabi-gcc -I../include -o selector selector.o

trial: trial.o
	arm-linux-gnueabi-gcc -I../include -o trial trial.o

rlocker : rlocker.o
	arm-linux-gnueabi-gcc -I../include -o rlocker rlocker.o

wlocker : wlocker.o
	arm-linux-gnueabi-gcc -I../include -o wlocker wlocker.o

runlocker : runlocker.o
	arm-linux-gnueabi-gcc -I../include -o runlocker runlocker.o

wunlocker : wunlocker.o
	arm-linux-gnueabi-gcc -I../include -o wunlocker wunlocker.o

clean: 
	rm -rf *.o $(OBJ) $(TESTER)
