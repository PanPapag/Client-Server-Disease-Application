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

all: $(BDIR)/whoClient $(BDIR)/master

$(BDIR)/whoClient: $(ALL_OBJ)
	$(CC) $(CLIENT_OBJ) $(COMMON_OBJ) -o $@ $(LFLAGS)
	rm -rf $(CLIENT_OBJ)

$(BDIR)/master: $(ALL_OBJ)
	$(CC) $(MASTER_OBJ) $(COMMON_OBJ) -o $@ $(LFLAGS)
	rm -rf $(MASTER_OBJ) $(COMMON_OBJ)

.PHONY: clean

clean:
	rm -rf $(BDIR) $(ALL_OBJ)

$(VERBOSE).SILENT:
