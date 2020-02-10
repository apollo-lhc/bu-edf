CXX=g++

CXXFLAGS=-Iinclude -std=c++11

LD_FLAGS=-lfreeipmi -lboost_program_options

SRC=src

DEPS=include/%.hh

OBJ= main.o net_helpers.o TimeSensor.o Sensor.o IpmiTemperatureSensor.o IpmiFanSpeedSensor.o

.PHONEY: all clean

all: main

main: $(OBJ)
	$(CXX) -o $@ $^ $(LD_FLAGS)

%.o: $(SRC)/%.cc 
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -f $(OBJ)
