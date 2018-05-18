OBJ = fork32 trial multithread multischedule fork
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

multischedule: multischedule.o
	$(CC) -I../inlcude -o $@ $<

fork: fork.o
	$(CC) -I../include -o $@ $<

clean: 
	rm -rf *.o $(OBJ)
