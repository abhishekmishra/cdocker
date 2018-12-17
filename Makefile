CC=gcc
CFLAGS=-std=c11 `pkg-config --cflags json-c` `pkg-config --cflags libcurl` -I./src -DLOG_USE_COLOR
LIBS=`pkg-config --libs json-c` `pkg-config --libs libcurl`
LDFLAGS=-fPIC -shared
OUT_DIR=./bin/release
OBJ_DIR=./obj
TEST_OBJ_DIR=./obj
SRC_DIR=./src
TEST_DIR=./test
MKDIR_P=mkdir -p
debug: CFLAGS += -DDEBUG -g -O0
debug: OUT_DIR = ./bin/debug
test: OUT_DIR = ./bin/debug
test: CFLAGS += `pkg-config --cflags cmocka`
test: LIBS+= `pkg-config --libs cmocka`

.PHONY: directories

all: 		directories clibdocker
debug: 		directories clibdocker
test:		debug test_clibdocker

## see https://stackoverflow.com/questions/1950926/create-directories-using-make-file
## for creating output directories
directories: $(OUT_DIR) $(OBJ_DIR)

$(OUT_DIR):
			$(MKDIR_P) $(OUT_DIR)

$(OBJ_DIR):
			$(MKDIR_P) $(OBJ_DIR)
			
# see https://www.gnu.org/software/make/manual/html_node/Wildcard-Function.html
# Get list of object files, with paths
OBJECTS := $(patsubst %.c,%.o,$(wildcard $(SRC_DIR)/*.c))

clibdocker:	$(OBJECTS) #$(OBJ_DIR)/main.o $(OBJ_DIR)/docker_connection_util.o $(OBJ_DIR)/docker_containers.o
			$(CC) $(CFLAGS) -o $(OUT_DIR)/$@ $^ $(LIBS)
			
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	
$(TEST_OBJ_DIR)/test_%.o: $(TEST_DIR)/test_%.c
	$(CC) $(CFLAGS) -c $< -o $@
	
TEST_OBJECTS := $(patsubst %.c,%.o,$(wildcard $(TEST_DIR)/*.c))

# Link with everything but main.o (because it contains another definition of main.	
test_clibdocker: $(TEST_OBJECTS) $(filter-out %main.o, $(OBJECTS))
			$(CC) $(CFLAGS) -o $(OUT_DIR)/$@ $^ $(LIBS)
			./bin/debug/test_clibdocker

clean:
			rm -f ./obj/*.o ./bin/release/clibdocker ./bin/release/clibdocker.exe ./bin/release/test_clibdocker ./bin/release/test_clibdocker.exe
			rm -f ./obj/*.o ./bin/debug/clibdocker ./bin/debug/clibdocker.exe ./bin/debug/test_clibdocker ./bin/debug/test_clibdocker.exe
			