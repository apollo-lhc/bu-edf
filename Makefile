CXX=g++

CXXFLAGS=-Iinclude -std=c++11

LD_FLAGS=-lboost_program_options

SRC=src
BUILD=build
BIN=bin

BASE_SRC=$(wildcard $(SRC)/base/*.cc)
BASE_OBJ=$(patsubst $(SRC)/%.cc,$(BUILD)/%.o,$(BASE_SRC))

ATCA_SRC=$(wildcard $(SRC)/atca/*.cc)
ATCA_OBJ=$(patsubst $(SRC)/%.cc,$(BUILD)/%.o,$(ATCA_SRC))

SHELF_MONITOR_OBJ=$(BUILD)/shelf_monitor.o $(BASE_OBJ) $(ATCA_OBJ)
SHELF_MONITOR_LIB=-lfreeipmi 

SHELF_SCAN_OBJ=$(BUILD)/shelf_scan.o $(BASE_OBJ) $(ATCA_OBJ)
SHELF_SCAN_LIB=-lfreeipmi 

.PHONEY: all clean dist_clean

all: $(BIN)/shelf_monitor $(BIN)/shelf_scan

$(BIN)/shelf_monitor: $(SHELF_MONITOR_OBJ)
	mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(LD_FLAGS) $(SHELF_MONITOR_LIB)

$(BIN)/shelf_scan: $(SHELF_SCAN_OBJ)
	mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(LD_FLAGS) $(SHELF_SCAN_LIB)

$(BUILD)/%.o: $(SRC)/%.cc
	mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILD)/%.o: $(SRC)/%.cxx
	mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -rf $(BUILD)
dist_clean: clean
	rm -rf $(BIN)
