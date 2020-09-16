#ifndef __APOLLO_BLADE_HH__
#define __APOLLO_BLADE_HH__
#include <base/SensorFactory.hh>
#include <base/Sensor.hh>
#include <vector>
#include <string>

class ApolloBlade : public Sensor {

public:

  ApolloBlade(std::vector<std::string> const &);

  //  virtual int Connect(std::string const &IP_addr, int port_number);
  
  virtual int Report();

  virtual float GetVal();


private:

  virtual int DoConnect();

  void SetModuleNumber(std::string const &);

  void SetDatabaseBaseString(std::string const &);
  
  void SetHostname(std::string const &);

  void SetDeviceAccessAddress(std::string const &);

  void SetSensors();

  std::string moduleNumber;

  std::string databaseBase;

  std::string hostname;

  std::string deviceAccessAddress;
  
  std::vector<Sensor *> apolloSensors;
};

//register the sensor
RegisterSensor(ApolloBlade,"ApolloBlade")
#endif
