#include <iostream>
#include <base/net_helpers.hh>
#include <base/SensorFactory.hh>
#include <unistd.h>
#include <string>
#include <fstream>
#include <boost/program_options.hpp>
#include <stdexcept>
#include <base/daemon.hh>
#include <signal.h>
#include <syslog.h>  

namespace po = boost::program_options;

#define DevFac SensorFactory::Instance()

std::vector<std::string> split_sensor_string(std::string sensor, std::string const & delimiter);

int main(int argc, char ** argv){

  std::string configFile("/etc/graphite_monitor");
  std::string runPath("/opt/address_tables");
  std::string pidFile("/var/run/graphite_monitor.pid");
  boost::program_options::options_description commandLineOptions{"Options"}; // for parsing command line
  commandLineOptions.add_options()
    ("config_file", boost::program_options::value<std::string>(),"config file")
    ("run_path" ,boost::program_options::value<std::string>(), "run path")
    ("pid_file" ,boost::program_options::value<std::string>(), "pid file");
  boost::program_options::variables_map commandLineVM; // for parsing command line 
  try {
    // parse command line
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, commandLineOptions), commandLineVM);
  } catch(const boost::program_options::error &ex) {
    fprintf(stderr, "Caught exception while parsing command line: %s \nTerminating <daemon name here>\n", ex.what());       
    return -1;
  }
 
  //check for CLI arguments
  if(commandLineVM.count("config_file")) {
    configFile = commandLineVM["config_file"].as<std::string>();
  }
  if(commandLineVM.count("run_path")) {
    runPath = commandLineVM["run_path"].as<std::string>();
  }
  if(commandLineVM.count("pid_file")) {
    pidFile = commandLineVM["pid_file"].as<std::string>();
  }  
  
  
  Daemon daemon;
  daemon.daemonizeThisProgram(pidFile, runPath);
  unsigned int secondsToSleep = 30;

  std::string IP_addr("127.0.0.1");
  int port_number = 2003; // plaintext data port

  // create map that we'll put sensors in
  std::vector<Sensor*> sensors;
  try {
    po::options_description fileOptions{"File"};

    fileOptions.add_options()
      ("sensor", po::value<std::string>(), "value")
      ("graphite_server", po::value<std::string>(), "value")
      ("graphite_port", po::value<std::string>(), "value")
      ("poll_period", po::value<std::string>(), "value");

    std::ifstream ifs(configFile.c_str());
    if(ifs){
      po::parsed_options config_options = po::parse_config_file(ifs, fileOptions);
      int num_options = config_options.options.size();


      for ( int iOption = 0; iOption < num_options; iOption++ ) {
	// values in config file are space-delimited
	std::string delimiter = " ";
	std::vector<std::string> sensorInfo;
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
	      syslog(LOG_INFO,"%s",e.what());
	    }
	  }else if(option == "graphite_server"){
	    IP_addr = config_options.options[iOption].value[0];
	  }else if(option == "graphite_port"){
	    port_number = atoi(config_options.options[iOption].value[0].c_str());
	  }else if (option == "poll_time"){
	    secondsToSleep = atoi(config_options.options[iOption].value[0].c_str());
	  }
	}catch (std::exception &e){
	  syslog(LOG_INFO,"Invalid line: %s",e.what());
	}
      }
    }
  } catch(const po::error &ex){
  }

  //Connect to the graphite server
  for(size_t iSensor = 0; iSensor < sensors.size();iSensor++){
    sensors[iSensor]->Connect(IP_addr, port_number);
  }
  


  // ============================================================================
  // Signal handling
  struct sigaction sa_INT,sa_TERM,old_sa;
  daemon.changeSignal(&sa_INT , &old_sa, SIGINT);
  daemon.changeSignal(&sa_TERM, NULL   , SIGTERM);
  daemon.SetLoop(true);
  
  
  int attempts = 0;
  int successful = 0;
  int attempts_timer = 0;
  // sleep and repeat this process
  while(daemon.GetLoop()){
    for (size_t i = 0; i < sensors.size(); i++ ) {
      attempts++;
      try{
	sensors[i]->Report();
	successful++;
      }catch(const std::exception & e){
      }
      
    }
    
    
    attempts_timer += secondsToSleep;
    if ( attempts_timer >= 600){
      syslog(LOG_INFO,"%d out of %d successful reports in the last 10 minutes\n", successful, attempts);
      attempts = 0;
      successful = 0;
      attempts_timer = 0;
    }
    
    
    sleep(secondsToSleep);
    
  }

  // Restore old action of receiving SIGINT (which is to kill program) before returning 
  sigaction(SIGINT, &old_sa, NULL);
  syslog(LOG_INFO,"eyescan Daemon ended\n");

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
