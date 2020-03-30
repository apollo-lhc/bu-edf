#include <stdio.h>
#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>
#include <ApolloReader.hh>


int main(int argc, char ** argv){

  char *hostname = "192.168.10.171";
  ipmi_ctx_t ipmiContext_ = ipmi_ctx_create();


  int connection = ipmi_ctx_open_outofband(ipmiContext_,
					   hostname,
					   "",
					   "",
					   IPMI_AUTHENTICATION_TYPE_NONE,
					   IPMI_PRIVILEGE_LEVEL_ADMIN,
					   0,0,0,0);

  /*  if (connection < 0){
    printf("bad connection\n");
    throw std::runtime_error(strerror(ipmi_ctx_errnum(ipmiContext_)));
    }*/

  uint8_t deviceAddr = 0x22;
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

  int raw_res_fru = 0;
  while(raw_res_fru >= 0){
    raw_res_fru = ipmi_cmd_raw_ipmb(ipmiContext_,
                                       channel_number,
                                       0x10,
                                       lun,
                                       net_fn,
                                       (void const *) buf_rq, buf_rq_size,
					buf_rs, buf_rs_size);
    int num_zeros = 0;
    for ( int i = 2; i < 5;i++){
      if (buf_rs[i] == 0x00) {
	num_zeros++;
      }
    }
    if(num_zeros == 3){
      break;
    }
    printf(" raw res %d,",raw_res_fru);
    printf("device found with fru_id %d\n", fru_id);
    fru_id++;
    buf_rq[1] = fru_id;
  }


  /*
  while(deviceAddr < 256){
    int raw_result = ipmi_cmd_raw_ipmb(ipmiContext_,
				       channel_number,
				       deviceAddr,
				       lun,
				       net_fn,
				       (void const *) buf_rq, buf_rq_size,
				       buf_rs, buf_rs_size);

     if (raw_result < 0){
      printf("nothing found at %02x\n", deviceAddr);
     } else {
       printf("Item found at %02x\n", deviceAddr);
       ApolloReader *apolloReader = new ApolloReader(hostname, deviceAddr);
       
       std::string board_manufacturer = apolloReader->GetBoardManufacturer();
       printf("board manufacturer: %s\n", board_manufacturer.c_str());
       std::string board_name = apolloReader->GetBoardName();
       printf("board name: %s\n",  board_name.c_str());
       std::string board_serial = apolloReader->GetBoardSerial();
       printf("board serial: %s\n", board_serial.c_str());
       std::string board_part_num = apolloReader->GetBoardPartNumber();
       printf("board part num: %s\n", board_part_num.c_str());
       std::string fru_file_id = apolloReader->GetFruFileId();
       printf("fru file id: %s\n", fru_file_id.c_str());
       
       std::string product_manufacturer = apolloReader->GetProductManufacturer();
       printf("product manufacturer: %s\n", product_manufacturer.c_str());
       std::string product_name = apolloReader->GetProductName();
       printf("product name: %s\n", product_name.c_str());
       std::string product_num = apolloReader->GetProductPartNumber();
       printf("product part number: %s\n", product_num.c_str());
       std::string product_version = apolloReader->GetProductVersion();
       printf("product version: %s\n", product_version.c_str());
       std::string product_serial = apolloReader->GetProductSerial();
       printf("product serial: %s\n", product_serial.c_str());
       std::string asset_tag = apolloReader->GetAssetTag();
       printf("asset tag: %s\n", asset_tag.c_str());
       printf("\n\n\n");
      }
 






      if (deviceAddr == 0xfe){
	break;
      }
      deviceAddr += 2;
      }*/
  printf("done\n");
  /*  ApolloReader *apolloReader = new ApolloReader(hostname, deviceAddr);

  std::string board_manufacturer = apolloReader->GetBoardManufacturer();
  printf("board manufacturer: %s\n", board_manufacturer.c_str());
  std::string board_name = apolloReader->GetBoardName();
  printf("board name: %s\n",  board_name.c_str());
  std::string board_serial = apolloReader->GetBoardSerial();
  printf("board serial: %s\n", board_serial.c_str());
  std::string board_part_num = apolloReader->GetBoardPartNumber();
  printf("board part num: %s\n", board_part_num.c_str());
  std::string fru_file_id = apolloReader->GetFruFileId();
  printf("fru file id: %s\n", fru_file_id.c_str());

  std::string product_manufacturer = apolloReader->GetProductManufacturer();
  printf("product manufacturer: %s\n", product_manufacturer.c_str());
  std::string product_name = apolloReader->GetProductName();
  printf("product name: %s\n", product_name.c_str());
  std::string product_num = apolloReader->GetProductPartNumber();
  printf("product part number: %s\n", product_num.c_str());
  std::string product_version = apolloReader->GetProductVersion();
  printf("product version: %s\n", product_version.c_str());
  std::string product_serial = apolloReader->GetProductSerial();
  printf("product serial: %s\n", product_serial.c_str());
  std::string asset_tag = apolloReader->GetAssetTag();
  printf("asset tag: %s\n", asset_tag.c_str());
  */
  //std::vector<uint8_t> apollo_data = apolloReader->GetData();
  /*
  for(int i = 0; i < apollo_data.size()/8; i++){
    printf("%03d   ", i*8);
    for(int j = 0; j < 8; j++){
      printf("%02x ",apollo_data[8*i + j]); 
    }
    printf("\n");
  }
  */

  
  

  return 0;

}
