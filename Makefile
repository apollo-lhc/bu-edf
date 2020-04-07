CXX=g++

CXXFLAGS=-Iinclude -std=c++11

LD_FLAGS=-lfreeipmi -lboost_program_options

SRC=src

DEPS=include/%.hh

BUILD=build

LIB=lib

OBJ_MONITOR=$(BUILD)/shelf_monitor.o \
	$(BUILD)/net_helpers.o \
	$(BUILD)/TimeSensor.o \
	$(BUILD)/Sensor.o \
	$(BUILD)/IpmiTemperatureSensor.o \
	$(BUILD)/IpmiFanSpeedSensor.o \
	$(BUILD)/ApolloBlade.o

OBJ_SCAN=$(BUILD)/shelf_scan.o\
	$(BUILD)/FruReader.o

.PHONEY: all clean

all: shelf_monitor shelf_scan

shelf_monitor: $(OBJ_MONITOR) 
	$(CXX) -o $@ $^ $(LD_FLAGS)

$(BUILD)/%.o: $(SRC)/%.cc
	mkdir -p $(BUILD)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILD)/%.o: $(SRC)/%.cxx
	mkdir -p $(BUILD)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

shelf_scan: $(OBJ_SCAN)
	$(CXX) -o $@ $^ $(LD_FLAGS)

clean:
	rm -f $(OBJ_MONITOR) $(OBJ_SCAN)
