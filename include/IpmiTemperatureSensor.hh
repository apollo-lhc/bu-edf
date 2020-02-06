#include <Sensor.hh>

class IpmiTemperatureSensor : public Sensor {

public:
  IpmiTemperatureSensor(int sensorNum, std::string dbName);

  void SetSensorNumber(int sensorNum);

  virtual float GetVal();

private:
  int sensorNumber;
};
