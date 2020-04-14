#include <ApolloMonitor/ApolloMonitor.hh>
#include <base/net_helpers.hh>
#include <string>
#include <vector>

int ApolloMonitor::Report() {
  try{
    std::string sensorVals = SM->GenerateGraphiteStatus(level,table);
    
    for(size_t iS = 0;
	iS < sensorVals.size();
	){
      //find the next "\n"
      size_t end = sensorVals.find_first_of("\n",iS);
      if(end == std::string::npos){
	break;
      }
      end++;
      //Adding apollo
      writen(sockfd, base.c_str(), base.size());
      //write "."
      writen(sockfd, ".", 1);
      //write the rest of the value
      writen(sockfd,sensorVals.c_str() + iS,end - iS);
      iS = end;
    }	
  }catch(BUException::exBase const & e){
    //syslog(LOG_ERR,"Caught BUException: %s\n   Info: %s\n",e.what(),e.Description());          
  }catch(std::exception const & e){
    //syslog(LOG_ERR,"Caught std::exception: %s\n",e.what());          
  }
}

float ApolloMonitor::GetVal(){
  return 0.0;
}

ApolloMonitor::ApolloMonitor(std::string const & connectionFile,
			     std::string const & userBase, 
			     std::string const & tableName, 
			     int displayLevel=1){
  level = displayLevel;
  table=tableName;
  SM = new ApolloSM();
  std::vector<std::string> arg;
  arg.push_back(connectionFile);
  SM->Connect(arg);
  //Get
  std::stringstream ss;
  ss << userBase ;
//     << "Apollo" 
//     << SM->Read("SLAVE_I2C.S8.IPMC_IP.BYTE_3");  //please fix me! (OK @ BU)
  base = ss.str();
}
