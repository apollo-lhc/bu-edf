#include <ApolloReader.hh>

#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>

ApolloReader::ApolloReader(){
  
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

  
}


std::vector<uint8_t> ApolloReader::GetHeader(){



}


