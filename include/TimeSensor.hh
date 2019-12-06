#include <Sensor.hh>

class TimeSensor : public Sensor {
  
public:
  TimeSensor();
  
  virtual float get_val();

};
