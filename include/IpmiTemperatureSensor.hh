#include <Sensor.hh>

class IpmiTemperatureSensor : public Sensor {

public:
  IpmiTemperatureSensor(int sensorNum, std::string dbName,  char *hostname_,  uint8_t deviceAddr);

  void SetSensorNumber(int sensorNum);

  void SetHostname(char *hostname_);

  void SetDeviceAccessAddress(uint8_t deviceAddr);

  //  virtual int Connect(std::string const &IP_addr, int port_number);

  //virtual int Report();
  
  virtual float GetVal();

private:
  int sensorNumber;

  char *hostname;

  uint8_t deviceAccessAddress;
};
