#include <stdio.h>
#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>
#include <ApolloReader.hh>


int main(int argc, char ** argv){

  char *hostname = "192.168.10.171";
  uint8_t deviceAddr = 0x88;
  
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
