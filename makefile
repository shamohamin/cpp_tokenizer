CC      :=  g++
STD  	:= c++17
SRC     := ./cpp_src
BIN     := ./bin
OBJ     := ./obj

MKDIR   := mkdir
RMDIR   := rm -rf

ifeq 	($(OS),Windows_NT)
	MKDIR   := mkdir
	RMDIR   := rmdir
else
	OS_NAME := $(shell uname)
	ifeq ($(OS_NAME),Linux)
		MKDIR   := mkdir
		RMDIR   := rm -rf
	endif
	ifeq ($OS_NAME),Darwin)
		MKDIR   := mkdir
		RMDIR   := rm -rf
	endif
endif

INCLUDE := ./$(SRC)/include
CFLAGS  := -I$(INCLUDE)
SRCS    :=$(wildcard $(SRC)/*.cpp)
OBJS    :=$(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(SRCS))
EXE     := $(BIN)/main

all: $(EXE)

$(EXE): $(OBJS) | $(BIN)
	$(CC) -std=$(STD) $^ -o $@ 

$(OBJ)/%.o: $(SRC)/%.cpp | $(OBJ)
	$(CC) -std=$(STD) $(CFLAGS) -c $< -o $@

$(BIN) $(OBJ):
	$(MKDIR) $@

run: $(EXE)
	$< $(ARG1) $(ARG2)
    
clean:
	$(RMDIR) $(BIN) $(OBJ)