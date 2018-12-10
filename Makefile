CC=gcc
CFLAGS=-std=c11 `pkg-config --cflags json-c` `pkg-config --cflags libcurl`
LIBS=`pkg-config --libs json-c` `pkg-config --libs libcurl`

all:		clibdocker

clibdocker:	main.o
			$(CC) $(CFLAGS) -o clibdocker main.o $(LIBS)

main.o:
			$(CC) $(CFLAGS) -c -o main.o main.c $(LIBS)

clean:
			rm *.o clibdocker