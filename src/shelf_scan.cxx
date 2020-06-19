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

int main(int argc, char ** argv){

   char *hostname;
   try {
     po::options_description desc{"Options"};
     desc.add_options()
       ("host", po::value<std::string>(), "value");

    po::variables_map vm;
    po::store(parse_command_line(argc, argv, desc),vm);
    po::notify(vm);


    if (vm.count("host")){
      hostname =  new char[vm["host"].as<std::string>().size() + 1];
      std::string str_hostname = vm["host"].as<std::string>();
      std::copy( str_hostname.begin(),  str_hostname.end(), hostname);
      hostname[str_hostname.size()] = '\0';
    } else {
      hostname = "192.168.10.171"; 
    }
    
    } catch ( const po::error &ex ){
     std::cerr << ex.what() << '\n';
    }
  
  ipmi_ctx_t ipmiContext_ = ipmi_ctx_create();

  //  printf("running shelf scanner for %s:\n", hostname); // do this if verbose
  int connection = ipmi_ctx_open_outofband(ipmiContext_,
					   hostname,
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
  
  uint8_t deviceAddr = 0x02;
			    
  uint8_t channel_number = 0;
  uint8_t lun = 0;
  uint8_t net_fn = 0xa; // 0xa is net fn for storage
  const size_t buf_rq_size = 2;
  uint8_t buf_rq[buf_rq_size];
  uint8_t read_code = 0x10;
  uint8_t fru_id = 0;

  buf_rq[0] = read_code;
  buf_rq[1] = fru_id;


  const size_t buf_rs_size = 256;
  uint8_t buf_rs[buf_rs_size];

  // check every ipmb address
  while(deviceAddr < 256){
    try{

      int raw_res_fru = 0;
      // check address until we reach invalid fru id
      while(raw_res_fru >= 0){

	FRUReader *apolloReader = new FRUReader(hostname, deviceAddr, fru_id);

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
	if(num_zeros == 3){
	  break;
	}

	apolloReader->PrintFRUInfo(false);
	fru_id++;
	buf_rq[1] = fru_id;
      }
      //      printf("here\n");
      //}


    } catch(std::runtime_error &e) {

    }
    if (deviceAddr == 0xfe){
      break;
    }
    
    fru_id = 0;
    buf_rq[1] = fru_id;
    deviceAddr += 2;
    
  }
  
  ipmi_ctx_close(ipmiContext_);
  
  //}
  return 0;
  
}
