#include <iostream>
#include <net_helpers.hh>
#include <Sensor.hh>
#include <TimeSensor.hh>
#include <IpmiTemperatureSensor.hh>
#include <IpmiFanSpeedSensor.hh>
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
  int port_number = 2003;

  char *shelf_hostname = "192.168.10.171";
  uint8_t rs_addr = 0x20;

  // create map that we'll put sensors in
  std::vector<Sensor*> sensors;

  try {
    po::options_description fileOptions{"File"};
    fileOptions.add_options()
      ("sensor", po::value<std::string>(), "value");
    
    std::string configFileName = "sensors.config";
    std::ifstream ifs{configFileName};


    if(ifs){
      po::parsed_options config_options = po::parse_config_file(ifs, fileOptions);
      int num_sensors = config_options.options.size();

      // values in config file are space-delimited
      std::string delimiter = " ";

      for ( int i = 0; i < num_sensors; i++ ) {

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
	int ipmi_sensor_number = std::stoi(sensor_info[2]); // this has to be c++ 11

	try {
	// make sensor objects. We currently can only make Ipmi Temp or Fanspeed sensors
	if (sensor_type == "IpmiTemperature") {
      	  Sensor *tempSensor = new IpmiTemperatureSensor(ipmi_sensor_number, wsp_filename, shelf_hostname, rs_addr);
	  sensors.push_back(tempSensor);
	  sensors[i]->Connect(IP_addr, port_number);
	} else if (sensor_type == "IpmiFanspeed") {
	  Sensor *fanspeedSensor = new IpmiFanSpeedSensor(ipmi_sensor_number, wsp_filename);
	  sensors.push_back(fanspeedSensor);
	  sensors[i]->Connect(IP_addr, port_number);
	} else {
	  printf("invalid sensor type: %s\n", sensor_type.c_str());
	}
	} catch (std::runtime_error &e){
	  std::cout << e.what() << std::endl;
	}
      }
    }
  } catch(const po::error &ex){
    std::cerr << ex.what() << '\n';
  }

  
    unsigned int secondsToSleep = 30;
    int seconds = 0;
    // sleep and repeat this process
    while(1){
      for ( int i = 0; i < sensors.size(); i++ ) {
	sensors[i]->Report();
      }

      //	printf("temperature and fanspeed reported, it has been %d seconds\n", seconds);
      seconds += secondsToSleep;
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




  /*  int tempSensorNumber1 = 235;
  std::string const &tempName1 = "Sensors.IpmiTemperatureSensor1";
  Sensor *ipmi_temperature_sensor_1 = new IpmiTemperatureSensor(tempSensorNumber1, tempName1);
  ipmi_temperature_sensor_1->Connect(IP_addr, port_number);
 
  int tempSensorNumber2 = 236;
  std::string const &tempName2 = "Sensors.IpmiTemperatureSensor2";
  Sensor *ipmi_temperature_sensor_2 = new IpmiTemperatureSensor(tempSensorNumber2, tempName2);
  ipmi_temperature_sensor_2->Connect(IP_addr, port_number);
  int tempSensorNumber3 = 237;
  std::string const &tempName3 = "Sensors.IpmiTemperatureSensor3";
  Sensor *ipmi_temperature_sensor_3 = new IpmiTemperatureSensor(tempSensorNumber3, tempName3);
  ipmi_temperature_sensor_3->Connect(IP_addr, port_number);

  int tempSensorNumber4 = 238;
  std::string const &tempName4 = "Sensors.IpmiTemperatureSensor4";
  Sensor *ipmi_temperature_sensor_4 = new IpmiTemperatureSensor(tempSensorNumber4, tempName4);
  ipmi_temperature_sensor_4->Connect(IP_addr, port_number);


  int fanspeedSensorNumber1 = 240;
  std::string const &fanspeedName1 = "Sensors.IpmiFanSpeedSensor1";
  Sensor *ipmi_fanspeed_sensor_1 = new IpmiFanSpeedSensor(fanspeedSensorNumber1, fanspeedName1);
  ipmi_fanspeed_sensor_1->Connect(IP_addr, port_number);

  int fanspeedSensorNumber2 = 241;
  std::string const &fanspeedName2 = "Sensors.IpmiFanSpeedSensor2";
  Sensor *ipmi_fanspeed_sensor_2 = new IpmiFanSpeedSensor(fanspeedSensorNumber2, fanspeedName2);
  ipmi_fanspeed_sensor_2->Connect(IP_addr, port_number);

  int fanspeedSensorNumber3 = 242;
  std::string const &fanspeedName3 = "Sensors.IpmiFanSpeedSensor3";
  Sensor *ipmi_fanspeed_sensor_3 = new IpmiFanSpeedSensor(fanspeedSensorNumber3, fanspeedName3);
  ipmi_fanspeed_sensor_3->Connect(IP_addr, port_number);

  int fanspeedSensorNumber4 = 243;
  std::string const &fanspeedName4 = "Sensors.IpmiFanSpeedSensor4";
  Sensor *ipmi_fanspeed_sensor_4 = new IpmiFanSpeedSensor(fanspeedSensorNumber4, fanspeedName4);
  ipmi_fanspeed_sensor_4->Connect(IP_addr, port_number);
  */
