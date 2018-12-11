CC=gcc
CFLAGS=-std=c11 `pkg-config --cflags json-c` `pkg-config --cflags libcurl`
LIBS=`pkg-config --libs json-c` `pkg-config --libs libcurl`

all:		clibdocker

clibdocker:	main.o docker_connection_util.o docker_containers.o
			$(CC) $(CFLAGS) -o clibdocker main.o docker_connection_util.o docker_containers.o $(LIBS)

main.o: docker_connection_util.o docker_containers.o main.c
			$(CC) $(CFLAGS) -c -o main.o main.c docker_connection_util.o docker_containers.o $(LIBS)
			
docker_containers.o: docker_connection_util.o docker_containers.h docker_containers.c
			$(CC) $(CFLAGS) -c -o docker_containers.o docker_connection_util.o docker_containers.c $(LIBS)
			
docker_connection_util.o: docker_connection_util.c docker_connection_util.h
			$(CC) $(CFLAGS) -c -o docker_connection_util.o docker_connection_util.c $(LIBS)

clean:
			rm *.o clibdocker