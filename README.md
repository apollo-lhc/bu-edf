# Grafana / Graphite Monitoring Code

This code monitors a range of sensors and pushes their information to a Graphite/Grafana server for plotting.

## shelf_monitor
This executable reads a config file (/etc/graphite_monitor or CL arg) that contains the Graphite connection information and a list of sensors to be built. 

The different kinds of sensors are added at link-time.   
The procedure for this is to first build the sensor libs you want, which will put the lib so files in lib/, then build the main executable which will link in all the libs in lib/.

 ```bash
 [root@apollo06 Grafana-Monitor]# make list
all				       distclean		lib/libATCA.so
bin/shelf_monitor		       lib_ApolloMonitor	shelf_monitor
bin/shelf_scan			       				lib_ATCA		shelf_scan
clean												lib/libApolloMonitor.so
```


```bash
[root@apollo06 Grafana-Monitor]# make lib_ApolloMonitor
g++ -c -o build/ApolloMonitor/ApolloMonitor.o src/ApolloMonitor/ApolloMonitor.cc -Iinclude -std=c++11 -fPIC -Wall -g -O3 -I/opt/BUTool/include -I/opt/cactus/include 
g++ -o lib/libApolloMonitor.so build/ApolloMonitor/ApolloMonitor.o -lboost_program_options -lboost_system -L/opt/BUTool/lib -Wl,-rpath=/opt/BUTool/lib -shared -fPIC -Wall -g -O3 -rdynamic -lBUTool_Helpers -lBUTool_ApolloSM -lToolException -lBUTool_IPBusIO 
```


```bash
[root@apollo06 Grafana-Monitor]# make
g++ -c -o build/shelf_monitor.o src/shelf_monitor.cxx -Iinclude -std=c++11 -fPIC -Wall -g -O3
g++ -c -o build/base/SensorFactory.o src/base/SensorFactory.cc -Iinclude -std=c++11 -fPIC -Wall -g -O3
src/base/SensorFactory.cc:8:13: warning: ‘bool CLIArgsValid(const string&, const string&)’ defined but not used [-Wunused-function]
 static bool CLIArgsValid(std::string const & flag,std::string const & full_flag){
             ^
g++ -c -o build/base/Sensor.o src/base/Sensor.cc -Iinclude -std=c++11 -fPIC -Wall -g -O3
g++ -c -o build/base/net_helpers.o src/base/net_helpers.cc -Iinclude -std=c++11 -fPIC -Wall -g -O3
build/shelf_monitor.o build/base/SensorFactory.o build/base/Sensor.o build/base/net_helpers.o
build/base/SensorFactory.o build/base/Sensor.o build/base/net_helpers.o
g++ -o bin/shelf_monitor build/shelf_monitor.o build/base/SensorFactory.o build/base/Sensor.o build/base/net_helpers.o -lboost_program_options -lboost_system -Llib -lApolloMonitor
```


