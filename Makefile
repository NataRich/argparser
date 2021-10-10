# Makefile (Win 10)

CC := gcc
CSTD := gnu99
INC_DIR := include
BIN_DIR := bin
SRC_DIR := src
OBJ_DIR := $(BIN_DIR)/obj
CFLAGS := -c -Wall -I$(INC_DIR)/  # -I shortens the include path to header files
COFLAG := -O2
CSTD := gnu99

SRCS := $(SRC_DIR)/main.c $(SRC_DIR)/argparser.c
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)  # substitution
DEPS := $(SRCS:$(SRC_DIR)/%.c=$(INC_DIR)/%.h)  # substitution

APPNAME := main
OUT := $(BIN_DIR)/$(APPNAME)

all: $(OUT)

# $@ replaces with the entire left part of :
# $^ replaces with the entire right part of :
$(OUT): $(OBJS)
	$(CC) -o $@ $(COFLAG) -std=$(CSTD) $^

# $< replaces with the first item in the dependency list
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -std=$(CSTD) $< -o $@

.PHONY: clean
clean:
	del .\$(BIN_DIR)\$(APPNAME).exe
	del .\$(BIN_DIR)\obj\*.o


