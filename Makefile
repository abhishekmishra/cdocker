CC=gcc
CFLAGS=-std=c11 `pkg-config --cflags json-c` `pkg-config --cflags libcurl`
LIBS=`pkg-config --libs json-c` `pkg-config --libs libcurl`
LDFLAGS=-fPIC -shared
OUT_DIR=./bin/release
OBJ_DIR=./obj
MKDIR_P=mkdir -p
debug: CFLAGS += -DDEBUG -g -O0
debug: OUT_DIR = ./bin/debug

.PHONY: directories

all: 		directories clibdocker
debug: 		directories clibdocker

## see https://stackoverflow.com/questions/1950926/create-directories-using-make-file
## for creating output directories
directories: $(OUT_DIR) $(OBJ_DIR)

$(OUT_DIR):
			$(MKDIR_P) $(OUT_DIR)

$(OBJ_DIR):
			$(MKDIR_P) $(OBJ_DIR)

clibdocker:	$(OBJ_DIR)/main.o $(OBJ_DIR)/docker_connection_util.o $(OBJ_DIR)/docker_containers.o
			$(CC) $(CFLAGS) -o $(OUT_DIR)/$@ $^ $(LIBS)
			
$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
			rm -f ./obj/*.o ./bin/release/clibdocker
			rm -f ./obj/*.o ./bin/debug/clibdocker