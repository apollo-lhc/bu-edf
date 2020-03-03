#include <iostream>
#include <net_helpers.hh>
#include <Sensor.hh>
#include <TimeSensor.hh>
#include <IpmiTemperatureSensor.hh>
#include <IpmiFanSpeedSensor.hh>
#include <ApolloBlade.hh>
#include <unistd.h>
#include <string>
#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>
#include <stdexcept>

namespace po = boost::program_options;


std::vector<std::string> split_sensor_string(std::string sensor, std::string delimiter);

int main(){

  std::string const &IP_addr = "127.0.0.1";
  int port_number = 2003; // plaintext data port

  // create map that we'll put sensors in
  std::vector<Sensor*> sensors;

  try {
    po::options_description fileOptions{"File"};

    fileOptions.add_options()
      ("sensor", po::value<std::string>(), "value")
      ("apollo", po::value<std::string>(), "value");

    std::string configFileName = "sensors.config";
    std::ifstream ifs{configFileName};


    if(ifs){

      po::parsed_options config_options = po::parse_config_file(ifs, fileOptions);
      int num_options = config_options.options.size();

      // values in config file are space-delimited
      std::string delimiter = " ";

      for ( int i = 0; i < num_options; i++ ) {
	std::string option = config_options.options[i].string_key;
	
	if(option == "sensor"){
	  // vector that we'll split the string into
	  std::vector<std::string> sensor_info;

	  // each element in config_options.options is a line in the config file
	  std::string sensor = config_options.options[i].value[0].c_str();
	  sensor_info = split_sensor_string(sensor, delimiter);

	  // currently sensor_type can be either IpmiTemperature or IpmiFanspeed
	  std::string sensor_type = sensor_info[0];
	  std::string fan_tray_number = sensor_info[1];

	  // wsp_filename is the path to the wsp file which grafana reads the data from
	  std::string wsp_filename = "Sensors." + sensor_type + "." + fan_tray_number;
	  int ipmi_sensor_number = std::stoi(sensor_info[2]); // this has to be c++ 11, check out atoi

	  // get the hostname of the machine where the sensor is located
	  char *shelf_hostname = (char *) sensor_info[3].c_str();
	  // get the IPMB address of the device within the machine
	  unsigned long rs_addr =  strtoul((char *) sensor_info[4].c_str(), NULL, 0); 
	  //      	  uint8_t rs_addr = (uint8_t) rs_addr_int;
      	  try {
	    // make sensor objects. We currently can only make Ipmi Temp or Fanspeed sensors
	    if (sensor_type == "IpmiTemperature") {
	      Sensor *tempSensor = new IpmiTemperatureSensor(ipmi_sensor_number, wsp_filename, shelf_hostname, rs_addr);
	      sensors.push_back(tempSensor);
	      sensors[i]->Connect(IP_addr, port_number);
	      // made esnsor of type ./..
	    } else if (sensor_type == "IpmiFanspeed") {
	      Sensor *fanspeedSensor = new IpmiFanSpeedSensor(ipmi_sensor_number, wsp_filename, shelf_hostname, rs_addr);
	      sensors.push_back(fanspeedSensor);
	      sensors[i]->Connect(IP_addr, port_number);
	    } else {
	      printf("invalid sensor type: %s\n", sensor_type.c_str());
	    }
	  } catch (std::runtime_error &e){
	    std::cout << e.what() << std::endl;
	  }
	}


	else if(option == "apollo") {
          // vector that we'll split the string into                                                                               
	  std::vector<std::string> apollo_info;
	  std::string apollo = config_options.options[i].value[0].c_str();
	  apollo_info = split_sensor_string(apollo, delimiter);
	  

	  std::string base_string = "Sensors." + apollo_info[0] + ".";
	  int ipmi_sensor_number = std::stoi(apollo_info[1]);
	  char *shelf_hostname = (char *) apollo_info[3].c_str();
	  unsigned long rs_addr = strtoul((char *) apollo_info[4].c_str(), NULL, 0);


	  Sensor *apolloBlade = new ApolloBlade(ipmi_sensor_number, base_string, shelf_hostname, rs_addr);
	  sensors.push_back(apolloBlade);
	  sensors[i]->Connect(IP_addr, port_number);
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
	sensors[i]->Report();
	successful++;
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


std::vector<std::string> split_sensor_string(std::string sensor, std::string delimiter){
  
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
