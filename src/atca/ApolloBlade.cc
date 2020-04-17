#include <atca/ApolloBlade.hh>
//#include <atca/IPMITempSensor.hh>
//#include <freeipmi/api/ipmi-api.h>
//#include <freeipmi/spec/ipmi-authentication-type-spec.h>
//#include <freeipmi/spec/ipmi-privilege-level-spec.h>
//#include <stdexcept>


ApolloBlade::ApolloBlade(std::vector<std::string> const & arg){
  /*
   [0] moduleNum
   [1] dbBase
   [2] hostname
   [3] deviceAddr
  */

  SetModuleNumber(arg[0]);
  SetDatabaseBaseString(arg[1]);
  SetHostname(arg[2]);
  SetDeviceAccessAddress(arg[3]);  
  SetSensors();
}

void ApolloBlade::SetModuleNumber(std::string const & val){
  //check for valid number
  moduleNumber = val;
}

void ApolloBlade::SetDatabaseBaseString(std::string const & val){
  databaseBase = val;
}

void ApolloBlade::SetHostname(std::string const & val){
  hostname = val;
}

void ApolloBlade::SetDeviceAccessAddress(std::string const & val){
  //check for valid number
  deviceAccessAddress = val;
}


void ApolloBlade::SetSensors(){

  //build a vector of arguments
  std::vector<std::string> args(5,"");
  args[0] = "";
  args[1] = databaseBase + ".Apollo" + moduleNumber;
  args[2] = "";
  args[3] = hostname;
  args[4] = deviceAccessAddress;

  std::vector<std::pair<std::string,std::string> > sensorNames;
  sensorNames.push_back(std::pair<std::string,std::string>("3",  "InternalTemp"));
  sensorNames.push_back(std::pair<std::string,std::string>("4",  "Top"));
  sensorNames.push_back(std::pair<std::string,std::string>("5",  "Bottom"));
  sensorNames.push_back(std::pair<std::string,std::string>("6",  "Middle"));
  sensorNames.push_back(std::pair<std::string,std::string>("7",  "CommandModuleMicroController"));
  sensorNames.push_back(std::pair<std::string,std::string>("8",  "Firefly"));
  sensorNames.push_back(std::pair<std::string,std::string>("9",  "CommandModuleFPGA"));
  sensorNames.push_back(std::pair<std::string,std::string>("10", "CommandModuleRegulator"));
  sensorNames.push_back(std::pair<std::string,std::string>("11", "CommandModuleMCU"));
  sensorNames.push_back(std::pair<std::string,std::string>("12", "CommandModuleFPGAVU"));
  sensorNames.push_back(std::pair<std::string,std::string>("13", "CommandModuleFPGAKU"));
  sensorNames.push_back(std::pair<std::string,std::string>("14", "Firefly2"));
  sensorNames.push_back(std::pair<std::string,std::string>("15", "CommandModuleRegulator2"));

  for(size_t iSensor; iSensor < sensorNames.size();iSensor++){
    args[0] = sensorNames[iSensor].second;
    args[2] = sensorNames[iSensor].first;

    Sensor * sensor = SensorFactory::Instance()->Create("IPMITemp",args);
    if(NULL != sensor){
      apolloSensors.push_back(sensor);
    }
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
