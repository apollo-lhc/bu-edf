#include <Sensor.hh>

class IpmiFanSpeedSensor : public Sensor {

public:
  IpmiFanSpeedSensor(int sensorNum, std::string dbName);

  void SetSensorNumber(int sensorNum);

  virtual float GetVal();

private:
  int sensorNumber;
};
