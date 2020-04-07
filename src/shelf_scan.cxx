#include <stdio.h>
#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>
#include <FruReader.hh>
#include <boost/program_options.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

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

  //  printf("running shelf scanner for %s:\n", hostname);
  int connection = ipmi_ctx_open_outofband(ipmiContext_,
					   hostname,
					   "",
					   "",
					   IPMI_AUTHENTICATION_TYPE_NONE,
					   IPMI_PRIVILEGE_LEVEL_ADMIN,
					   0,0,0,0);

    if (connection < 0){
      printf("bad connection\n");
      //throw std::runtime_error(strerror(ipmi_ctx_errnum(ipmiContext_)));
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



  
  while(deviceAddr < 256){
    int raw_result = ipmi_cmd_raw_ipmb(ipmiContext_,
				       channel_number,
				       deviceAddr,
				       lun,
				       net_fn,
				       (void const *) buf_rq, buf_rq_size,
				       buf_rs, buf_rs_size);

     if (raw_result < 0){
      deviceAddr += 2;
     } else {

       int raw_res_fru = 0;
       while(raw_res_fru >= 0){
	 raw_res_fru = ipmi_cmd_raw_ipmb(ipmiContext_,
					 channel_number,
					 deviceAddr,
					 lun,
					 net_fn,
					 (void const *) buf_rq, buf_rq_size,
					 buf_rs, buf_rs_size);


	 //	 printf("Device found at 0x%02x, fru_id %d\n", deviceAddr, fru_id);	 

	 
	 int num_zeros = 0;
	 for ( int i = 2; i < 5;i++){
	   if (buf_rs[i] == 0x00) {
	     num_zeros++;
	   }
	 }
	 if(num_zeros == 3){
	   break;
	 }

	 
         FruReader *apolloReader = new FruReader(hostname, deviceAddr, fru_id);
	 apolloReader->PrintFruInfo(false);
	 /*
	 std::string board_manufacturer = apolloReader->GetBoardManufacturer();
	 if(board_manufacturer != ""){
	   printf("board manufacturer: %s\n", board_manufacturer.c_str());
	 }
         std::string board_name = apolloReader->GetBoardName();
	 if(board_name != ""){
	   printf("board name: %s\n",  board_name.c_str());
	 }
         std::string board_serial = apolloReader->GetBoardSerial();
	 if(board_serial != ""){
	   printf("board serial: %s\n", board_serial.c_str());
	 }
         std::string board_part_num = apolloReader->GetBoardPartNumber();
	 if(board_part_num != ""){
	   printf("board part num: %s\n", board_part_num.c_str());
	 }
         std::string fru_file_id = apolloReader->GetFruFileId();
	 if(fru_file_id != ""){
	   printf("fru file id: %s\n", fru_file_id.c_str());
         }
	 
         std::string product_manufacturer = apolloReader->GetProductManufacturer();
	 if(product_manufacturer != ""){
	   printf("product manufacturer: %s\n", product_manufacturer.c_str());
	 }
         std::string product_name = apolloReader->GetProductName();
	 if(product_name != ""){
	   printf("product name: %s\n", product_name.c_str());
	 }
         std::string product_num = apolloReader->GetProductPartNumber();
	 if(product_num != ""){
	   printf("product part number: %s\n", product_num.c_str());
	 }
         std::string product_version = apolloReader->GetProductVersion();
	 if(product_version != ""){
	   printf("product version: %s\n", product_version.c_str());
	 }
         std::string product_serial = apolloReader->GetProductSerial();
	 if(product_serial != ""){
	   printf("product serial: %s\n", product_serial.c_str());
	 }
         std::string asset_tag = apolloReader->GetAssetTag();
	 if(asset_tag != ""){
	   printf("asset tag: %s\n", asset_tag.c_str());
	 }*/

      
	 fru_id++;
	 buf_rq[1] = fru_id;
         	 
	 if (deviceAddr == 0xfe){
	   break;
	 }
       }
       fru_id = 0;
       buf_rq[1] = fru_id;
       deviceAddr += 2;
     }
     if (deviceAddr == 0xfe){
       break;
     }
  }
  return 0;

}
