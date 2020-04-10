#include <ApolloSM.hh>
#include <uhal/uhal.hpp>

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
  try{

  }catch(BUException::exBase const & e){
    syslog(LOG_ERR,"Caught BUException: %s\n   Info: %s\n",e.what(),e.Description());          
  }catch(std::exception const & e){
    syslog(LOG_ERR,"Caught std::exception: %s\n",e.what());          
  }
}

float ApolloBlade::GetVal(){
  return 0.0;
}

ApolloBlade::ApolloBlade(int moduleNum, std::string dbBase, char *hostname_, uint8_t deviceAddr){
  SetDatabaseBaseString(dbBase);
  SetHostname(hostname_);
  SM = new ApolloSM();
  std::vector<std::string> arg;
  arg.push_back("connections.xml");
  SM->Connect(arg);
}
