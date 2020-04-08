#include <base/Sensor.hh>

class IPMIFanSpeedSensor : public Sensor {

public:
  IPMIFanSpeedSensor(int sensorNum, std::string dbName, char *hostname, uint8_t deviceAddr);

  void SetSensorNumber(int sensorNum);

  void SetHostname(char *hostname_);

  void SetDeviceAccessAddress(uint8_t deviceAddr);

  virtual float GetVal();

private:
  int sensorNumber;

  char *hostname;

  uint8_t deviceAccessAddress;
};
