#include <Sensor.hh>
#include <vector>

class ApolloBlade : public Sensor {

public:

  ApolloBlade(int moduleNum, std::string dbBase, char *hostname_, uint8_t deviceAddr);

  void SetModuleNumber(int moduleNum);

  void SetDatabaseBaseString(std::string dbBase);
  
  void SetHostname(char *hostname_);

  void SetDeviceAccessAddress(uint8_t deviceAddr);

  virtual int Connect(std::string const &IP_addr, int port_number);
  
  virtual int Report();

  virtual float GetVal();
private:

  void SetSensors();

  int moduleNumber;

  std::string databaseBase;

  char *hostname;

  uint8_t deviceAccessAddress;
  
  std::vector<Sensor *> apolloSensors;
};
