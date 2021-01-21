CC      :=  g++
STD  	:= c++14
SRC     := ./cpp_src
BIN     := ./bin
OBJ     := ./obj
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
	mkdir $@

run: $(EXE)
	$<
    
clean:
	rm -rf $(BIN) $(OBJ)