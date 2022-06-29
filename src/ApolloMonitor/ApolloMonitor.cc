#include <ApolloMonitor/ApolloMonitor.hh>
#include <base/net_helpers.hh>
#include <string>
#include <vector>
#include <stdexcept>

#include <syslog.h>

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
      if(writen(sockfd, base.c_str(), base.size()) < 0){
	if( DoConnect() < 0){
	  syslog(LOG_WARNING,"connect error");
	  close(sockfd);
	  sockfd = -1;
	  return -1;
	}
      }
      //write "."
      writen(sockfd, ".", 1);
      //write the rest of the value
      writen(sockfd,sensorVals.c_str() + iS,end - iS);
      sensorCount++;
      iS = end;
      }	
  }catch(BUException::exBase const & e){
      syslog(LOG_ERR,"Caught BUException: %s\n   Info: %s\n",e.what(),e.Description());          
  }catch(std::exception const & e){
      syslog(LOG_ERR,"Caught std::exception: %s\n",e.what());          
  }
  return sensorCount;
}

float ApolloMonitor::GetVal(){
  return 0.0;
}

static std::string ReplaceInString(std::string original,
				   std::string const & replace,
				   std::string const & replacement){
  std::string ret;
  size_t lastPos = 0;
  size_t findPos = original.find(replace);
  while((findPos != std::string::npos) &&
	(findPos < original.size())){
    //append from the last position to this found position                                      
    ret.append(original.begin()+lastPos,
	       original.begin()+findPos);
    //append the replacement value
    ret.append(replacement);
    //move the last pointer, to the find pointer plus the replace size
    lastPos = findPos + replace.size();
    findPos = original.find(replace,lastPos);
  }
  //append the end of the string
  ret.append(original.begin() + lastPos,
	     original.end());
  return ret;
}                                                    

void ApolloMonitor::ReplaceSN(std::string & original){  
  char buffer[20];
  snprintf(buffer,20,"%02d",SM->GetSerialNumber());
  original = ReplaceInString(original,"$SN",buffer);  
}

void ApolloMonitor::ReplaceRN(std::string & original){  
  char buffer[20];
  snprintf(buffer,20,"%02d",SM->GetRevNumber());
  original = ReplaceInString(original,"$RN",buffer);  
}

void ApolloMonitor::ReplaceShelfID(std::string & original){  
  char buffer[20];
  snprintf(buffer,20,"%02d",SM->GetShelfID());
  original = ReplaceInString(original,"$SHELF_ID",buffer);  
}

void ApolloMonitor::ReplaceZynqIP(std::string & original){  
  char buffer[20];
  snprintf(buffer,20,"%u-%u-%u-%u",  // "-" are because graphite splits on "."s
	   (SM->GetZynqIP()>>24)&0xFF,
	   (SM->GetZynqIP()>>16)&0xFF,
	   (SM->GetZynqIP()>> 8)&0xFF,
	   (SM->GetZynqIP()>> 0)&0xFF
	   );
  original = ReplaceInString(original,"$ZYNQ_IP",buffer);  
}

void ApolloMonitor::ReplaceIPMCIP(std::string & original){  
  char buffer[20];
  snprintf(buffer,20,"%u-%u-%u-%u", // "-" are because graphite splits on "."s
	   (SM->GetIPMCIP()>>24)&0xFF,
	   (SM->GetIPMCIP()>>16)&0xFF,
	   (SM->GetIPMCIP()>> 8)&0xFF,
	   (SM->GetIPMCIP()>> 0)&0xFF
	   );
  original = ReplaceInString(original,"$IPMC_IP",buffer);  
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
  std::vector<std::string> connArgs;
  connArgs.push_back(args[0]);
  SM = new ApolloSM(connArgs);

  //Build base name using apollo SN
  base = args[1];
  //Do any replacements we need to do
  ReplaceSN(base);
  ReplaceRN(base);
  ReplaceShelfID(base);
  ReplaceZynqIP(base);
  ReplaceIPMCIP(base);

}
