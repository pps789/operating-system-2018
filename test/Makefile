OBJ = gpsupdate file_loc fopen_test fopen_holds
CC = arm-linux-gnueabi-gcc

%.o: %.c
	$(CC) -c -I../include -o $@ $<

all: $(OBJ)

gpsupdate: gpsupdate.o
	$(CC) -I../include -o $@ $<
	
file_loc: file_loc.o
	$(CC) -I../include -o $@ $< 

fopen_test: fopen_test.o
	$(CC) -I../include -o $@ $<

fopen_holds: fopen_holds.o
	$(CC) -I../include -o $@ $<

clean: 
	rm -rf *.o $(OBJ)
