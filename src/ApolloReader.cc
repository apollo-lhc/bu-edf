#include <ApolloReader.hh>

#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>

ApolloReader::ApolloReader(char *hostname_, uint8_t deviceAddr){
  hostname = hostname_;
  deviceAccessAddress = deviceAddr;
}

/*ApolloReader::~ApolloReader(){
  
}
*/


void ApolloReader::Read(){

  ipmi_ctx_t ipmiContext_ = ipmi_ctx_create();

  char *hostname = "192.168.10.171";

  int connection = ipmi_ctx_open_outofband(ipmiContext_,
					   hostnme,
					   "",
					   "",
					   IPMI_AUTHENTICATION_TYPE_NONE,
					   IPMI_PRIVILEGE_LEVEL_ADMIN,
					   0,0,0,0);

  if (connection < 0){
    printf("bad connection\n");
    throw std::runtime_error(strerror(ipmi_ctx_errnum(ipmiContext)));
  }


  
  uint8_t channel_number = 0;
  uint8_t lun = 0;
  uint8_t net_fn = 0xa; // 0xa is net fn for storage
  const size_t buf_rq_size = 5;

  uint8_t buf_rq[buf_rq_size];

  uint8_t read_code = 0x11;
  uint8_t fru_id = 0;
  uint8_t starting_byte_ls = 0;
  uint8_t starting_byte_ms = 0;
  uint8_t num_bytes_to_read = 8;

  buf_rq[0] = read_code;
  buf_rq[1] = fru_id;
  buf_rq[2] = starting_byte_ls;
  buf_rq[3] = starting_byte_ms;
  buf_rq[4] = num_bytes_to_read;

  const size_t buf_rs_size = 256;

  while (starting_byte_ls < 248){

    printf("%03d       ", starting_byte_ls);

     
    int raw_result = ipmi_cmd_raw_ipmb(ipmiContext_,
				       channel_number,
				       0x88,
				       lun,
				       net_fn,
				       (void const *) buf_rq, buf_rq_size,
				       buf_rs, buf_rs_size);

    
    /* bytes 3 through 3+num_bytes_to_read are the useful information in buf_rs*/

    

    if(starting_byte_ls == 0 && starting_byte_ms == 0){

      for(int i = 3; i < 3+num_bytes_to_read; i++){
	printf("header[%d] = %d\n", i, buf_rs[i]);
	header.push_back(buf_rs[i]);
      }
    }
    
  }

  
}


std::vector<uint8_t> ApolloReader::GetHeader(){



}


