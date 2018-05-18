OBJ = fork32 trial multithread
CC = arm-linux-gnueabi-gcc

%.o: %.c
	$(CC) -c -I../include -o $@ $<

all : $(OBJ)

fork32: fork32.o
	$(CC) -I../include -o $@ $<

trial: trial.o
	$(CC) -I../include -o $@ $<

multithread: multithread.o
	$(CC) -I../include -o $@ $< -lpthread

clean: 
	rm -rf *.o $(OBJ)
