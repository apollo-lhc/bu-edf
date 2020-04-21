#include <ApolloMonitor/ApolloMonitor.hh>
#include <base/net_helpers.hh>
#include <string>
#include <vector>
#include <stdexcept>

int ApolloMonitor::Report() {
  int sensorCount = 0;
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
      sensorCount++;
      iS = end;
    }	
  }catch(BUException::exBase const & e){
    //syslog(LOG_ERR,"Caught BUException: %s\n   Info: %s\n",e.what(),e.Description());          
  }catch(std::exception const & e){
    //syslog(LOG_ERR,"Caught std::exception: %s\n",e.what());          
  }
  return sensorCount;
}

float ApolloMonitor::GetVal(){
  return 0.0;
}

ApolloMonitor::ApolloMonitor(std::vector<std::string> const & args){
  /*
  args:
    [0] connectionFile,
    [1] userBase, 
    [2] tableName, 
    [3] displayLevel
   */
  if(args.size() < 4){
    throw std::runtime_error("Too few arguments");
  }  

  //status display parameters
  level= atoi(args[3].c_str());
  table= args[2];

  //Create and initialize ApolloSM
  SM = new ApolloSM();
  std::vector<std::string> connArgs;
  connArgs.push_back(args[0]);
  SM->Connect(connArgs);

  //Build base name using apollo SN
  std::stringstream ss;
  ss << args[1] ;
//     << "Apollo" 
//     << SM->Read("SLAVE_I2C.S8.IPMC_IP.BYTE_3");  //please fix me! (OK @ BU)
  base = ss.str();
}
