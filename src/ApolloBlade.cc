#include <ApolloBlade.hh>
#include <IpmiTemperatureSensor.hh>
#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>



ApolloBlade::ApolloBlade(int moduleNum, std::string dbBase, char *hostname_, uint8_t deviceAddr){
  SetModuleNumber(moduleNum);
  SetDatabaseBaseString(dbBase);
  SetHostname(hostname_);
  SetDeviceAccessAddress(deviceAddr);
  
  SetSensors();
}

void ApolloBlade::SetModuleNumber(int moduleNum){
  moduleNumber = moduleNum;
}

void ApolloBlade::SetDatabaseBaseString(std::string dbBase){
  databaseBase = dbBase;
}

void ApolloBlade::SetHostname(char *hostname_){
  hostname = hostname_;
}

void ApolloBlade::SetDeviceAccessAddress(uint8_t deviceAddr){
  deviceAccessAddress = deviceAddr;
}


void ApolloBlade::SetSensors(){
  
  Sensor *U34 = new IpmiTemperatureSensor(4, "Sensors.IpmiTemperature.ApolloU34", hostname, deviceAccessAddress);
  apolloSensors.push_back(U34);
  Sensor *U35 = new IpmiTemperatureSensor(5, "Sensors.IpmiTemperature.ApolloU35", hostname, deviceAccessAddress);
  apolloSensors.push_back(U35);
  Sensor *U36 = new IpmiTemperatureSensor(6, "Sensors.IpmiTemperature.ApolloU36", hostname, deviceAccessAddress);
  apolloSensors.push_back(U36);

}

int ApolloBlade::Connect(std::string const &IP_addr, int port_number) {
  for(int i = 0; i < apolloSensors.size(); i++){
    if(apolloSensors[i]->Connect(IP_addr, port_number) < 0) {
      return -1;
    }
  }
  return 0;
}

int ApolloBlade::Report() {
  for(int i = 0; i < apolloSensors.size(); i++){
    if(int report = apolloSensors[i]->Report() < 0){
      return -1;
    }
  }
  return 0;
}

float ApolloBlade::GetVal(){
  return 0.0;
}
