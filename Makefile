CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pthread -Iinclude

SRC_DIR := src
OBJ_DIR := obj
BIN := main

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN)

run: all
	./$(BIN)

.PHONY: all clean run

