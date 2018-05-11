OBJ = fork32
CC = arm-linux-gnueabi-gcc
%.o: %.c
	$(CC) -c -I../include -o $@ $<
all : $(OBJ)

fork32: fork32.o
	$(CC) -I../include -o $@ $<

clean: 
	rm -rf *.o $(OBJ)
