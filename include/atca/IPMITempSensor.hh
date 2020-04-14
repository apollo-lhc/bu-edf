#ifndef __IPMI_TEMP_SENSOR_HH__
#define __IPMI_TEMP_SENSOR_HH__
#include <base/SensorFactory.hh>
#include <base/Sensor.hh>

class IPMITempSensor : public Sensor {

public:
  IPMITempSensor(std::vector<std::string> const &);

  
  virtual float GetVal();

private:
  void SetSensorNumber(std::string const &);
  void SetHostname(std::string const &);
  void SetDeviceAccessAddress(std::string const &);

  int sensorNumber;

  std::string hostname;

  uint8_t deviceAccessAddress;
};

//register the sensor
RegisterSensor(IPMITempSensor,"IPMITemp")
#endif
