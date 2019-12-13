#include <iostream>
#include <net_helpers.hh>
#include <Sensor.hh>
#include <TimeSensor.hh>
#include <unistd.h>



int main(){

  std::string const &IP_addr = "127.0.0.1";
  int port_number = 2003;

  std::string const &name = "Sensors.TimeSensor";

  Sensor *time_sensor_1 = new TimeSensor();

  time_sensor_1->Connect(IP_addr, port_number);

  time_sensor_1->SetDatabaseName(name);

  unsigned int secondsToSleep = 60;
  int minutes = 0;
  // sleep and repeat this process
  while(1){
    time_sensor_1->Report();
    printf("time reported, it has been %d minutes\n", minutes);
    minutes++;
    sleep(60);
  }

  if (NULL != time_sensor_1){
    delete time_sensor_1;
  }

  return 0;
}
