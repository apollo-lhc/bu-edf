#include <base/Sensor.hh>
#include <ApolloSM/ApolloSM.hh>

class ApolloMonitor : public Sensor {

public:
  ApolloMonitor(char *hostname_, std::string connectionFile);
  virtual int Report();
  virtual float GetVal();
private:
  void SetSensors();
  std::string base;
  std::string hostname;
  ApolloSM * SM;
  std::string ApolloID;
};
