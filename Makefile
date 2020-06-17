BDIR := build

CC := gcc
CFLAGS := -std=gnu99 -g3
LFLAGS := -lpthread

$(shell mkdir -p $(BDIR))

ALL_SRC := $(shell find . -type f -regex ".*\.c")
ALL_OBJ := $(patsubst %.c, %.o, $(ALL_SRC))

COMMON_OBJ := $(filter ./common/%.o, $(ALL_OBJ))
CLIENT_OBJ := $(filter ./client/%.o, $(ALL_OBJ))
MASTER_OBJ := $(filter ./master/%.o, $(ALL_OBJ))
WORKER_OBJ := $(filter ./worker/%.o, $(ALL_OBJ))
SERVER_OBJ := $(filter ./server/%.o, $(ALL_OBJ))

all: $(BDIR)/master $(BDIR)/worker $(BDIR)/whoClient $(BDIR)/whoServer

$(BDIR)/master: $(ALL_OBJ)
	$(CC) $(MASTER_OBJ) $(COMMON_OBJ) -o $@ $(LFLAGS)
	rm -rf $(MASTER_OBJ)

$(BDIR)/worker: $(ALL_OBJ)
	$(CC) $(WORKER_OBJ) $(COMMON_OBJ) -o $@ $(LFLAGS)
	rm -rf $(WORKER_OBJ)

$(BDIR)/whoClient: $(ALL_OBJ)
	$(CC) $(CLIENT_OBJ) $(COMMON_OBJ) -o $@ $(LFLAGS)
	rm -rf $(CLIENT_OBJ)

$(BDIR)/whoServer: $(ALL_OBJ)
	$(CC) $(SERVER_OBJ) $(COMMON_OBJ) -o $@ $(LFLAGS)
	rm -rf $(SERVER_OBJ) $(COMMON_OBJ)

.PHONY: clean

clean:
	rm -rf $(BDIR) $(ALL_OBJ)

$(VERBOSE).SILENT:
