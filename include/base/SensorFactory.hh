#ifndef __SENSOR_FACTORY_H__
#define __SENSOR_FACTORY_H__

#include <base/Sensor.hh>

#define RegisterSensor(ClassName,ClassNickName)				\
  namespace {								\
    /*make creator function*/						\
    Sensor * creator_function(std::vector<std::string> args){		\
      return new ClassName(args);					\
    }									\
    /*Register the device with the DeviceFactory*/			\
    const char type[] = #ClassName;					\
    const char name[] = ClassNickName;					\
    const bool registered = SensorFactory::Instance()->Register(type,	\
								name,	\
								&creator_function); \
    }

#endif

class SensorFactory{
public:
  //Singleton stuff
  static SensorFactory * Instance() {
    if(NULL == pInstance){
      pInstance = new SensorFactory;
    }
    return pInstance;
  }; 

  bool Register(std::string  type,
		std::string  name, 
		Sensor * (*fPtr)(std::vector<std::string>));
  void UnRegister(std::string  name);
  Sensor * Create(std::string,std::vector<std::string>);
private:
  //Singleton stuff
  //Never implement
  SensorFactory(){};
  SensorFactory(const SensorFactory &);
  SensorFactory & operator=(const SensorFactory &);
  
  ~SensorFactory();
  static SensorFactory * pInstance;

  //List stuff
  struct sSensor{
    std::string type;
    Sensor * (*fPtr)(std::vector<std::string>);
  };
  std::map<std::string,sSensor> sensorMap;
};
