#include <ApolloMonitor/ApolloMonitor.hh>
#include <base/net_helpers.hh>
#include <string>
#include <vector>

int ApolloMonitor::Report() {
  try{
    std::string sensorVals = SM->GenerateGraphiteStatus(3,"CM_MON");

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

ApolloMonitor::ApolloMonitor(char *hostname_, std::string connectionFile){
  base = "Apollo06";  
  hostname = hostname_;
  SM = new ApolloSM();
  std::vector<std::string> arg;
  arg.push_back(connectionFile);
  SM->Connect(arg);
}
