

CXX=g++
SRC=src
BUILD=build
BIN=bin
LIB=lib
INSTALL_PATH?=./install

CXX_FLAGS=-Iinclude -std=c++11 -fPIC -Wall -g -O3
LD_FLAGS=-lboost_program_options -lboost_system

#================================================================================
#== SHELF_MONITOR
#================================================================================
SHELF_MONITOR_LD_FLAGS=$(LD_FLAGS) -L$(LIB)
SHELF_MONITOR_LIBS=""
SHELF_MONITOR_OBJ=$(BUILD)/shelf_monitor.o

#================================================================================
#== Sensor's and Sensor factory
#================================================================================
SENSOR_BASE_SRC=$(wildcard $(SRC)/base/*.cc)
SENSOR_BASE_OBJ=$(patsubst $(SRC)/%.cc,$(BUILD)/%.o,$(SENSOR_BASE_SRC))
SHELF_MONITOR_OBJ += $(SENSOR_BASE_OBJ)

#================================================================================
#== FreeIPMI based Sensor
#================================================================================
ATCA_LIB=$(LIB)/libATCA.so
ATCA_SRC=$(wildcard $(SRC)/atca/*.cc)
ATCA_OBJ=$(patsubst $(SRC)/%.cc,$(BUILD)/%.o,$(ATCA_SRC))
ATCA_LIBS=-lfreeipmi 
ATCA_LD_FLAGS=-shared -fPIC -Wall -g -O3 -rdynamic
ifneq ("$(wildcard $(ATCA_LIB))","")
	SHELF_MONITOR_LIB+=-lATCA
endif

#================================================================================
#== Apoll Monitor Sensor
#================================================================================
CACTUS_ROOT?=/opt/cactus
APOLLO_PATH?=/opt/BUTool
APOLLO_MONITOR_LIB=$(LIB)/libApolloMonitor.so
APOLLO_MONITOR_SRC=$(wildcard $(SRC)/ApolloMonitor/*.cc)
APOLLO_MONITOR_OBJ=$(patsubst $(SRC)/%.cc,$(BUILD)/%.o,$(APOLLO_MONITOR_SRC))
APOLLO_MONITOR_LIBS= -lBUTool_Helpers \
			-lBUTool_ApolloSM \
			-lToolException \
			-lBUTool_IPBusIO 
APOLLO_MONITOR_CXX_FLAGS=-I$(APOLLO_PATH)/include -I$(CACTUS_ROOT)/include 
APOLLO_MONITOR_LD_FLAGS=-L$(APOLLO_PATH)/lib -Wl,-rpath=$(APOLLO_PATH)/lib -shared -fPIC -Wall -g -O3 -rdynamic
ifneq ("$(wildcard $(APOLLO_MONITOR_LIB))","")
	SHELF_MONITOR_LIB+=-lApolloMonitor
endif

#================================================================================
#== Apoll Monitor Sensor
#================================================================================
SHELF_SCAN_OBJ=$(BUILD)/shelf_scan.o $(BASE_OBJ) $(ATCA_OBJ) 
SHELF_SCAN_LIB=-lfreeipmi
#================================================================================


.PHONEY: all clean distclean shelf_monitor shelf_scan lib_ApolloMonitor lib_ATCA list install
all: shelf_monitor 

#================================================================================
#== Aliases
#================================================================================
shelf_monitor: $(BIN)/shelf_monitor
shelf_scan: $(BIN)/shelf_scan
lib_ApolloMonitor: $(APOLLO_MONITOR_LIB)
lib_ATCA: $(ATCA_LIB)

#================================================================================
#== EXEs
#================================================================================
$(BIN)/shelf_monitor: $(SHELF_MONITOR_OBJ)
	@mkdir -p $(dir $@)
	@echo $(SHELF_MONITOR_OBJ)
	@echo $(SENSOR_BASE_OBJ)
	$(CXX) -o $@ $^ $(SHELF_MONITOR_LD_FLAGS) $(SHELF_MONITOR_LIB) $(INSTALL_LD_FLAGS)

$(BIN)/shelf_scan: $(SHELF_SCAN_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(LD_FLAGS) $(SHELF_SCAN_LIB)

#================================================================================
#== Sensor Libs
#================================================================================
$(APOLLO_MONITOR_LIB) : $(APOLLO_MONITOR_OBJ) 
	@mkdir -p $(LIB)
	$(CXX) -o $@ $^ $(LD_FLAGS) $(APOLLO_MONITOR_LD_FLAGS) $(APOLLO_MONITOR_LIBS)

$(ATCA_LIB) : $(ATCA_OBJ)
	@mkdir -p $(LIB)
	$(CXX) -o $@ $^ $(LD_FLAGS) $(ATCA_LD_FLAGS) $(ATCA_LIBS)

#================================================================================
#== Generic rules
#================================================================================
#Build cc files
$(BUILD)/%.o: $(SRC)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(CXX_FLAGS)
#Build cxx files
$(BUILD)/%.o: $(SRC)/%.cxx
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(CXX_FLAGS)
#build ApolloMonitor cc files 
$(BUILD)/ApolloMonitor/%.o: $(SRC)/ApolloMonitor/%.cc
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(CXX_FLAGS) $(APOLLO_MONITOR_CXX_FLAGS)

#================================================================================
#== Install
#================================================================================

install: $(SHELF_MONITOR_OBJ)
	rm -f $(BIN)/shelf_monitor
	make $(BIN)/shelf_monitor INSTALL_LD_FLAGS='-Wl,-rpath=$(INSTALL_PATH)/lib'
	install -m 775 -d $(INSTALL_PATH)/lib
	install -m 775 -d $(INSTALL_PATH)/bin
	install -b -m 775 $(BIN)/shelf_monitor ${INSTALL_PATH}/bin
	install -b -m 775 $(LIB)/* ${INSTALL_PATH}/lib

#================================================================================
#== Cleanup & helpers
#================================================================================
clean:
	rm -rf $(BUILD)
distclean: clean
	rm -rf $(BIN)
	rm -rf $(LIB)

#list magic: https://stackoverflow.com/questions/4219255/how-do-you-get-the-list-of-targets-in-a-makefile                                                                
list:
	@$(MAKE) -pRrq -f $(lastword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | sort | egrep -v -e '^[^[:alnum:]]' -e '^$@$$' | column

