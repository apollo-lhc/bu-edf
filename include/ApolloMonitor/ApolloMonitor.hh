#ifndef __APOLLO_MONITOR_HH__
#define __APOLLO_MONITOR_HH__
#include <base/SensorFactory.hh>
#include <base/Sensor.hh>
#include <ApolloSM/ApolloSM.hh>

class ApolloMonitor : public Sensor {

public:
  ApolloMonitor(std::vector<std::string> const & args);
  virtual int Report();
  virtual float GetVal();
private:
  void ReplaceSN(std::string &);
  void ReplaceRN(std::string &);
  void ReplaceShelfID(std::string &);
  void ReplaceZynqIP(std::string &);
  void ReplaceIPMCIP(std::string &);

  void SetSensors();
  std::string base;
  ApolloSM * SM;
  int level;
  std::string table;
};

RegisterSensor(ApolloMonitor,"ApolloMonitor")
#endif
