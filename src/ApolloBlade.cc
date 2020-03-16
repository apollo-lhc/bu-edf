#include <ApolloBlade.hh>
#include <IpmiTemperatureSensor.hh>
#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>
#include <iostream>


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

  Sensor *internal = new IpmiTemperatureSensor(3, databaseBase + "InternalTemp", hostname, deviceAccessAddress);
  if (internal != NULL){
    apolloSensors.push_back(internal);
  }
  Sensor *U34 = new IpmiTemperatureSensor(4, databaseBase + "Top", hostname, deviceAccessAddress);
  if (U34 != NULL){
    apolloSensors.push_back(U34);
  }
  Sensor *U35 = new IpmiTemperatureSensor(5, databaseBase +  "Bottom", hostname, deviceAccessAddress);
  if(U35 != NULL){
    apolloSensors.push_back(U35);
  }
  Sensor *U36 = new IpmiTemperatureSensor(6, databaseBase + "Middle", hostname, deviceAccessAddress);
  if(U36 != NULL){
    apolloSensors.push_back(U36);
  }
  Sensor *CMmicroController = new IpmiTemperatureSensor(7, databaseBase + "CommandModuleMicroController", hostname, deviceAccessAddress);
  if(CMmicroController != NULL){
    apolloSensors.push_back(CMmicroController);
  }
  Sensor *FF = new IpmiTemperatureSensor(8, databaseBase + "Firefly", hostname, deviceAccessAddress);
  if(CMmicroController != NULL){
    apolloSensors.push_back(FF);
  }
  Sensor *CMfpga = new IpmiTemperatureSensor(9, databaseBase + "CommandModuleFPGA", hostname, deviceAccessAddress);
  if(CMfpga != NULL){
    apolloSensors.push_back(CMfpga);
  }
  Sensor *CMregulator = new IpmiTemperatureSensor(10, databaseBase + "CommandModuleRegulator", hostname, deviceAccessAddress);
  if(CMfpga != NULL){
    apolloSensors.push_back(CMfpga);
  }
  Sensor *CMmcu = new IpmiTemperatureSensor(11, databaseBase + "CommandModuleMCU", hostname, deviceAccessAddress);
  if(CMmcu != NULL){
    apolloSensors.push_back(CMmcu);
  }
  Sensor *CMfpgaVU = new IpmiTemperatureSensor(12, databaseBase + "CommandModuleFPGAVU", hostname, deviceAccessAddress);
  if(CMfpgaVU != NULL){
    apolloSensors.push_back(CMfpgaVU);
  }
  Sensor *CMfpgaKU = new IpmiTemperatureSensor(13, databaseBase + "CommandModuleFPGAKU", hostname, deviceAccessAddress);
  if(CMfpgaKU != NULL){
    apolloSensors.push_back(CMfpgaKU);
  }
  Sensor *FF2 = new IpmiTemperatureSensor(14, databaseBase + "Firefly2", hostname, deviceAccessAddress);
  if(FF2 != NULL){
    apolloSensors.push_back(FF2);
  }
  Sensor *CMregulator2 = new IpmiTemperatureSensor(15, databaseBase + "CommandModuleRegulator2", hostname, deviceAccessAddress);
  if(CMregulator2 != NULL){
    apolloSensors.push_back(CMregulator2);
  }
}

int ApolloBlade::Connect(std::string const &IP_addr, int port_number) {
  for(int i = 0; i < apolloSensors.size(); i++){
    // need to throw exception if one of these fails
    if(apolloSensors[i]->Connect(IP_addr, port_number) < 0) {
      return -1;
    }
  }
  return 0;
}

int ApolloBlade::Report() {
  for(int i = 0; i < apolloSensors.size(); i++){
    try {
      apolloSensors[i]->Report();
    } catch (std::runtime_error &e) {

    }
  }
  return 0;
}

float ApolloBlade::GetVal(){
  return 0.0;
}
