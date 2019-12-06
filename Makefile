#IDIR=include

#CC=g++
#CXX_FLAGS = -I${IDIR}


#obj/%.o : src/%.cc
#	${CC} ${CXX_FLAGS} -c $< -o $@



#main: src/main.cc src/writen.cc src/Sensor.cc src/TimeSensor.cc
#	$(CC) ${CXX_FLAGS} -o main src/main.cc



#bin/% : src/%.o 
#	mkdir -p bin
#	${CC} $^ -o $@

CFLAGS=-Iinclude

SRC=src

DEPS=include/%.hh

OBJ= net_helpers.o TimeSensor.o Sensor.o main.o

%.o: $(SRC)/%.cc $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $A $^ $(CLAGS)
