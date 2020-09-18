#include <stdio.h>
#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>
#include <atca/FRUReader.hh>
#include <boost/program_options.hpp>
#include <stdexcept>
#include <string>
#include <iostream>

namespace po = boost::program_options;

char GetSpinnyThing(){
  static char spinnyThing[] = {'-','\\','|','/'};
  static int sel;
  //cycle through the 4 options
  sel++;
  sel &= 0x3;
  return spinnyThing[sel];
}

int main(int argc, char ** argv){

  std::string hostname;
  try {
    po::options_description cliOptions{"Options"};
    cliOptions.add_options()
      ("help,h", "Help Screen")
      ("host", po::value<std::string>(), "Shelf Manager hostname");

    po::variables_map allOptions;
    po::store(parse_command_line(argc, argv, cliOptions),allOptions);
    po::notify(allOptions);

    //Bail quickly if "help" was specified 
    if(allOptions.find("help") != allOptions.end()){
      std::cout << cliOptions << std::endl;
      return 0;
    }


    if (allOptions.count("host")){
      hostname = allOptions["host"].as<std::string>();
    } else {
      std::cout << "Missing \"host\" argument" << std::endl;
      std::cout << cliOptions << std::endl;
      return 0;
    }
    
  } catch ( const po::error &ex ){
    std::cerr << ex.what() << '\n';
  }
  
  ipmi_ctx_t ipmiContext_ = ipmi_ctx_create();

  //  printf("running shelf scanner for %s:\n", hostname); // do this if verbose
  int connection = ipmi_ctx_open_outofband(ipmiContext_,
					   hostname.c_str(),
					   "",
					   "",
					   IPMI_AUTHENTICATION_TYPE_NONE,
					   IPMI_PRIVILEGE_LEVEL_ADMIN,
					   0,0,0,0);

  if (connection < 0){
    printf("bad connection!!!!\n");
    throw std::runtime_error(strerror(ipmi_ctx_errnum(ipmiContext_)));
    return 0;
  }
  

			    
  uint8_t channel_number = 0;
  uint8_t lun = 0;
  uint8_t net_fn = 0xa; // 0xa is net fn for storage
  const size_t buf_rq_size = 2;
  uint8_t buf_rq[buf_rq_size];
  uint8_t read_code = 0x10;
  uint16_t fru_id = 0;

  buf_rq[0] = read_code;
  buf_rq[1] = fru_id;


  const size_t buf_rs_size = 256;
  uint8_t buf_rs[buf_rs_size];
  //Vector of found FRUs
  std::vector<FRUReader*> FRUs;
  
  // check every ipmb address
  const size_t dotCountMax = 80;  
  printf("Searching...\n[");
  for(size_t iDot = 0;iDot < 256/(256/dotCountMax);iDot++){
    printf("=");
  }
  printf("]\n[.");
  size_t deltaDot = 256/dotCountMax;
  size_t nextDot = deltaDot;
  for(  uint16_t deviceAddr = 0x02; deviceAddr <= 0xFF;){
    try{
      if(deviceAddr > nextDot){
	printf(".");
	fflush(stdout);
	nextDot+=deltaDot;
      }
      int raw_res_fru = 0;
      // check address until we reach invalid fru id
      while(raw_res_fru >= 0){
	printf("%c",GetSpinnyThing());
	fflush(stdout);
	// check at this address and fru id for how many bytes are stored
	raw_res_fru = ipmi_cmd_raw_ipmb(ipmiContext_,
					channel_number,
					deviceAddr,
					lun,
					net_fn,
					(void const *) buf_rq, buf_rq_size,
					buf_rs, buf_rs_size);
	
	// if the raw ipmb command above gives 00 00 00, there is no device there
	int num_zeros = 0;
	for ( int i = 2; i < 5;i++){
	  if (buf_rs[i] == 0x00) {
	    num_zeros++;
	  }
	}
	printf("%c",8);
	fflush(stdout);
	if(num_zeros == 3){
	  break;
	}
	FRUReader *FRU = new FRUReader(hostname.c_str(), deviceAddr, fru_id);
	FRUs.push_back(FRU);

	fru_id++;
	buf_rq[1] = fru_id;
      }


    } catch(std::runtime_error &e) {

    }
    if (deviceAddr == 0xfe){
      break;
    }
    
    fru_id = 0;
    buf_rq[1] = fru_id;
    deviceAddr += 2;
    
  }
  printf("]\n\n\n");
  ipmi_ctx_close(ipmiContext_);

  printf("Shelf contents\n");
  printf("IPMB(FRU) | %*s | %*s |\n",60,"Product Name",10,"SN");
  printf("=======================================================================================\n");
  for(size_t iFRU = 0; iFRU < FRUs.size();iFRU++){
    FRUs[iFRU]->PrintFRUInfo(false);
  }
  
	
  return 0;
  
}
