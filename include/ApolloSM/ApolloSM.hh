#include <base/Sensor.hh>
#include <vector>
#include <ApolloSM/ApolloSM.hh>

class ApollSM : public Sensor {

public:
  ApollSM(char *hostname_, std::string connectionFile);
  virtual int Connect(std::string const &IP_addr, int port_number);
  virtual int Report();
  virtual float GetVal();
private:
  void SetSensors();
  std::string databaseBase;
  char *hostname;
  ApolloSM * SM;
};
