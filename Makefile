BDIR := build

CC := gcc
CFLAGS := -std=gnu99 -g3
LFLAGS := -lpthread

$(shell mkdir -p $(BDIR))

ALL_SRC := $(shell find . -type f -regex ".*\.c")
ALL_OBJ := $(patsubst %.c, %.o, $(ALL_SRC))
CLIENT_OBJ_FILTER := server_utils.o server.o
OBJ_SERVER := $(filter-out $(SERVER_OBJ_FILTER), $(ALL_OBJ))
OBJ_CLIENT := $(filter-out $(CLIENT_OBJ_FILTER), $(ALL_OBJ))

all: $(BDIR)/whoClient

$(BDIR)/whoClient: $(ALL_OBJ)
	$(CC) $(OBJ_CLIENT) -o $@ $(LFLAGS)
	rm -rf $(ALL_OBJ)

.PHONY: clean

clean:
	rm -rf $(BDIR)

$(VERBOSE).SILENT:
