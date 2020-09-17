#include <base/SensorFactory.hh>
#include <boost/algorithm/string/case_conv.hpp>

//Singleton
SensorFactory * SensorFactory::pInstance = NULL;


//static bool CLIArgsValid(std::string const & flag,std::string const & full_flag){
//  return (!flag.empty() && !full_flag.empty());
//}

bool SensorFactory::Register(std::string type, 
			     std::string name, 
			     Sensor * (*fPtr)(std::vector<std::string>)){
  boost::algorithm::to_upper(name);
  std::map<std::string,sSensor>::iterator it = sensorMap.find(name);
  
  if(it == sensorMap.end()){
    //Register a device for the first time
    sSensor & sensor = sensorMap[name];

    sensor.type = type;
    sensor.fPtr = fPtr;

    printf("Registered Sensor: %s\n",name.c_str());          
  }else{
    //This name is already registered.
    //This can happen since multiple files can include the header. 
    if(0 != it->second.type.compare(type)){
      //This is trying to register a sensorice with the same name, but different type.
      //This is very bad and we are going to explode.
      throw std::runtime_error("Sensor type name collision");      
    }
  }

  //Register command line options
  

  return true;
}

void SensorFactory::UnRegister(std::string name){
  boost::algorithm::to_upper(name);
  std::map<std::string,sSensor>::iterator it = sensorMap.find(name);
  if(it != sensorMap.end()){
    sensorMap.erase(it);
  }
}

Sensor * SensorFactory::Create(std::string name ,std::vector<std::string> args){
  boost::algorithm::to_upper(name);
  std::map<std::string,sSensor>::iterator it = sensorMap.find(name);
  if(it == sensorMap.end()){
    throw std::runtime_error("Unkown Sensor type");
  }

  Sensor * (*fptr)(std::vector<std::string>) = it->second.fPtr;
  Sensor * ret = (*fptr)(args);
  return ret;
  
}

std::vector<std::string> SensorFactory::GetTypes(){
  std::vector<std::string> ret;
  for(auto it = sensorMap.begin();it != sensorMap.end();it++){
    ret.push_back(it->first);
  }
  return ret;
}
