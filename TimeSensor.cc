#include <time.h>
#include <stdio.h>

TimeSensor::TimeSensor(){
  setDatabaseName("Sensors.TimeSensor");
}


float TimeSensor::get_val(){

  time_t t = time(NULL);

  return float(t);
}
