CC := g++
CFLAGS := -std=c++0x -g -O2 -static-libgcc -static-libstdc++
BIN_DIR := bin
BUILD_DIR := build
SRC_DIR := src
MAIN := WavFileTool
TARGET := wavfiletool.exe
SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

$(BIN_DIR)/$(TARGET): CREATE_DIRS $(BUILD_DIR)/$(MAIN).o $(OBJECTS) 
	$(CC) $(OBJECTS) $(CFLAGS)  -o $@ 

$(BUILD_DIR)/$(MAIN).o: $(SRC_DIR)/WavFileTool.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

CREATE_DIRS: 
	if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

CLEAN:
	if exist $(BUILD_DIR) rmdir /Q /S $(BUILD_DIR)