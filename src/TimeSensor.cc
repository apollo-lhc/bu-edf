#include <time.h>
#include <stdio.h>
#include <TimeSensor.hh>

TimeSensor::TimeSensor(){
  SetDatabaseName("Sensors.TimeSensor");
}


float TimeSensor::GetVal(){

  time_t t = time(NULL);

  return float(t);
}
