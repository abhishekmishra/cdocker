CC=gcc
CFLAGS=-std=c11 `pkg-config --cflags json-c` `pkg-config --cflags libcurl`
LIBS=`pkg-config --libs json-c` `pkg-config --libs libcurl`
DEBUG_FLAGS=-g -O0
LDFLAGS=-fPIC -shared

all:		clibdocker

clibdocker:	main.o docker_connection_util.so docker_containers.so
			$(CC) $(CFLAGS) -o clibdocker main.o docker_connection_util.so docker_containers.so $(LIBS) $(DEBUG_FLAGS)

main.o: docker_connection_util.so docker_containers.so main.c
			$(CC) $(CFLAGS) -c -o main.o main.c docker_connection_util.so docker_containers.so $(LIBS) $(DEBUG_FLAGS)
			
docker_containers.so: docker_connection_util.so docker_containers.h docker_containers.c
			$(CC) $(CFLAGS) -c -o docker_containers.so docker_connection_util.so docker_containers.c $(LIBS) $(LDFLAGS) $(DEBUG_FLAGS)
			
docker_connection_util.so: docker_connection_util.c docker_connection_util.h
			$(CC) $(CFLAGS) -c -o docker_connection_util.so docker_connection_util.c $(LIBS) $(LDFLAGS) $(DEBUG_FLAGS)

clean:
			rm *.o clibdocker