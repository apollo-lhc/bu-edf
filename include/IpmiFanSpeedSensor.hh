#include <Sensor.hh>

class IpmiFanSpeedSensor : public Sensor {

public:
  IpmiFanSpeedSensor(int sensorNum, std::string dbName, char *hostname, uint8_t deviceAddr);

  void SetSensorNumber(int sensorNum);

  void SetHostname(char *hostname_);

  void SetDeviceAccessAddress(uint8_t deviceAddr);

  virtual float GetVal();

private:
  int sensorNumber;

  char *hostname;

  uint8_t deviceAccessAddress;
};
