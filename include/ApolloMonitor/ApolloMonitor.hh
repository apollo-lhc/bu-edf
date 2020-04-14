#include <base/SensorFactory.hh>
#include <base/Sensor.hh>
#include <ApolloSM/ApolloSM.hh>

class ApolloMonitor : public Sensor {

public:
  ApolloMonitor(std::vector<std::string> const & args);
  virtual int Report();
  virtual float GetVal();
private:
  void SetSensors();
  std::string base;
  ApolloSM * SM;
  int level;
  std::string table;
};

RegisterSensor(ApolloMonitor,"ApolloMonitor")
