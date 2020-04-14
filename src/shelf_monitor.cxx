#include <iostream>
#include <base/net_helpers.hh>
#include <base/SensorFactory.hh>
#include <unistd.h>
#include <string>
#include <fstream>
#include <boost/program_options.hpp>
#include <stdexcept>

namespace po = boost::program_options;

#define DevFac SensorFactory::Instance()

std::vector<std::string> split_sensor_string(std::string sensor, std::string const & delimiter);

int main(int argc, char ** argv){

  std::string const &IP_addr = "192.168.10.20";//"127.0.0.1";
  int port_number = 2003; // plaintext data port

  // create map that we'll put sensors in
  std::vector<Sensor*> sensors;
  try {
    po::options_description fileOptions{"File"};

    fileOptions.add_options()
      ("sensor", po::value<std::string>(), "value");
    std::string configFileName = "example.config";
    if(argc > 1){
      configFileName=argv[1];
    }
    std::ifstream ifs{configFileName};
    if(ifs){
      po::parsed_options config_options = po::parse_config_file(ifs, fileOptions);
      int num_options = config_options.options.size();


      for ( int iOption = 0; iOption < num_options; iOption++ ) {
	// values in config file are space-delimited
	std::string delimiter = " ";
	std::vector<std::string> sensorInfo;
	printf("%s\n",config_options.options[iOption].value[0].c_str());
	sensorInfo = split_sensor_string(config_options.options[iOption].value[0],delimiter);     
	
	try{
	  std::string option = config_options.options[iOption].string_key;	
	  if(option == "sensor"){
	    // vector that we'll split the string into
	    std::vector<std::string> sensorInfo;	    
	    // each element in config_options.options is a line in the config file
	    sensorInfo = split_sensor_string(config_options.options[iOption].value[0].c_str(), delimiter);
	    std::string type = sensorInfo[0];
	    sensorInfo.erase(sensorInfo.begin()); //remove the name
	      
	    try {
	      Sensor * newSensor = DevFac->Create(type,sensorInfo);
	      if(newSensor){
		sensors.push_back(newSensor);
	      }
	    } catch (std::exception &e){
	      std::cout << e.what() << std::endl;
	    }
	  }
	}catch (std::exception &e){
	  std::cout << e.what() << " Invalid line?" << std::endl;
	}
      }
    }
  } catch(const po::error &ex){
    std::cerr << ex.what() << '\n';
  }

  
  unsigned int secondsToSleep = 30;
  
  
  int attempts = 0;
  int successful = 0;
  int attempts_timer = 0;
  // sleep and repeat this process
  while(1){
    for ( int i = 0; i < sensors.size(); i++ ) {
      attempts++;
      try{
	sensors[i]->Report();
	successful++;
      }catch(const std::exception & e){
      }
      
    }
    
    
    attempts_timer += secondsToSleep;
    if ( attempts_timer >= 600){
      printf("%d out of %d successful reports in the last 10 minutes\n", successful, attempts);
      attempts = 0;
      successful = 0;
      attempts_timer = 0;
    }
    
    
    sleep(secondsToSleep);
    
  }
  
  /*if (NULL != ipmi_temperature_sensor_1){
    delete ipmi_temperature_sensor_1;
    }*/
  
  return 0;
}


std::vector<std::string> split_sensor_string(std::string sensor, std::string const & delimiter){
  
  size_t position = 0;
  std::string token;
  std::vector<std::string> sensor_info;
  while( (position = sensor.find(delimiter)) != std::string::npos) {
    token = sensor.substr(0, position);
    sensor_info.push_back(token);
    sensor.erase(0, position+delimiter.length());
  }
  sensor_info.push_back(sensor);

  return sensor_info;
}
