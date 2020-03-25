#include <stdio.h>
#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>
#include <ApolloReader.hh>


int main(int argc, char ** argv){

  char *hostname = "192.168.10.171";
  uint8_t deviceAddr = 0x88;
  
  ApolloReader *apolloReader = new ApolloReader(hostname, deviceAddr);

  apolloReader->Read();

  std::vector<uint8_t> apollo_data = apolloReader->GetData();

  for(int i = 0; i < apollo_data.size()/8; i++){
    printf("%03d   ", i*8);
    for(int j = 0; j < 8; j++){
      printf("%02x ",apollo_data[8*i + j]); 
    }
    printf("\n");
  }



  
  /*  ipmi_ctx_t ipmiContext_ = ipmi_ctx_create();

  char *hostname = "192.168.10.171";


  /*  int connection = ipmi_ctx_open_outofband(ipmiContext_,
					   hostname,
					   "",
					   "",
					   IPMI_AUTHENTICATION_TYPE_NONE,
					   IPMI_PRIVILEGE_LEVEL_ADMIN,
					   0,0,0,0);

  if(connection < 0){
    printf("Failure\n");
    return -1;
    }*/


  /*
  
  uint8_t channel_number = 0;
  uint8_t lun = 0;
  uint8_t net_fn = 0xa;
  const size_t buf_rq_size = 5;

  uint8_t buf_rq[buf_rq_size];


  uint8_t read_code = 0x11;
  uint8_t fru_id = 0;
  uint8_t starting_byte_ls = 0;
  uint8_t starting_byte_ms = 0;
  uint8_t num_bytes_to_read = 8;
  buf_rq[0] = read_code;
  buf_rq[1] = fru_id;

  buf_rq[3] = starting_byte_ms;
  buf_rq[4] = num_bytes_to_read;

  const size_t buf_rs_size = 256;



  /* header fields */
  /*uint8_t header_format_version;
  uint8_t internal_use_area_offset;
  uint8_t chassis_info_area_offset;
  uint8_t board_area_offset;
  uint8_t product_info_area_offset;
  uint8_t multi_record_area_offset;
  uint8_t pad;
  uint8_t header_checksum;

  while (starting_byte_ls < 248){
    //    printf("%03d       ", starting_byte_ls);
    
    int connection = ipmi_ctx_open_outofband(ipmiContext_,
					     hostname,
					     "",
					     "",
					     IPMI_AUTHENTICATION_TYPE_NONE,
					     IPMI_PRIVILEGE_LEVEL_ADMIN,
					     0,0,0,0);
    if(connection < 0){
      printf("connection Failure\n");
      return -1;
    }
    buf_rq[2] = starting_byte_ls;

    uint8_t buf_rs[buf_rs_size];
    int raw_result = ipmi_cmd_raw_ipmb( ipmiContext_,
					channel_number,
					0x88,
					lun,
					net_fn,
					(void const *) buf_rq, buf_rq_size,
					buf_rs, buf_rs_size);



    if(starting_byte_ls == 0){
      header_format_version = buf_rs[3]*8;
      internal_use_area_offset = buf_rs[4]*8;
      chassis_info_area_offset = buf_rs[5]*8;
      board_area_offset = buf_rs[6]*8;
      product_info_area_offset = buf_rs[7]*8;
      multi_record_area_offset = buf_rs[8]*8;
      pad = buf_rs[9]*8;
      header_checksum = buf_rs[10]*8;


      if(chassis_info_area_offset){
	printf("chassis offset, bytes: %d\n", chassis_info_area_offset);
      } else {
	printf("NO CHASSIS OFFSET\n");
      }
      if(board_area_offset){
	printf("board_area_offset, bytes: %d\n", board_area_offset);
      } else {
	printf("NO BOARD INFO\n");
      }
      if(product_info_area_offset){
	printf("product_info_area_offset, bytes: %d\n", product_info_area_offset);
      } else {
	printf("NO PRODUCT INFO\n");
      }
      if(multi_record_area_offset){
	printf("multi_record_area_offset, bytes: %d\n", multi_record_area_offset);
      } else {
	printf("NO MULTI RECORD INFO\n");
      }

    }
    

    
    starting_byte_ls += 8;

    if(raw_result == -1 || raw_result == 0){
      printf("raw result no good\n");
      return -1;
    }

    ipmi_ctx_close(ipmiContext_);
    /*
    for(int i = 3; i < 11; i++){
      printf("%02x ", buf_rs[i]); 
    }
    printf("\n");
    }
  */
  
  return 0;

}
