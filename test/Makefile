%.o: %.c
	arm-linux-gnueabi-gcc -c -I../include -o $@ $<


gpsupdate: gpsupdate.o
	arm-linux-gnueabi-gcc -I../include -o gpsupdate gpsupdate.o
file_loc: file_loc.o
	arm-linux-gnueabi-gcc -I../include -o file_loc file_loc.o

all: gpsupdate file_loc


clean: 
	rm -rf *.o gpsupdate file_loc
