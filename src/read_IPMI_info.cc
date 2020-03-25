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
