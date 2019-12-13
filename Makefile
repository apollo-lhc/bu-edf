CXX=g++

CXXFLAGS=-Iinclude

SRC=src

DEPS=include/%.hh

OBJ= main.o net_helpers.o TimeSensor.o Sensor.o

.PHONEY: all clean

all: main

main: $(OBJ)
	$(CXX) -o $@ $^ $(CXXLAGS)

%.o: $(SRC)/%.cc 
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -f $(OBJ)
