#ifndef __IPMI_FAN_SPEED_SENSOR_HH__
#define __IPMI_FAN_SPEED_SENSOR_HH__

#include <base/SensorFactory.hh>
#include <base/Sensor.hh>

class IPMIFanSpeedSensor : public Sensor {

public:
  IPMIFanSpeedSensor(std::vector<std::string> const &);

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
RegisterSensor(IPMIFanSpeedSensor,"IPMIFanSpeed")

#endif
