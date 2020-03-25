#include <ApolloReader.hh>

#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>
#include <stdexcept>
#include <string.h>

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
					   hostname,
					   "",
					   "",
					   IPMI_AUTHENTICATION_TYPE_NONE,
					   IPMI_PRIVILEGE_LEVEL_ADMIN,
					   0,0,0,0);

  if (connection < 0){
    printf("bad connection\n");
    throw std::runtime_error(strerror(ipmi_ctx_errnum(ipmiContext_)));
  }

  ReadInformationLength(ipmiContext_);


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
  uint8_t buf_rs[buf_rs_size];

  /* use this command to get header information */
  int raw_result = ipmi_cmd_raw_ipmb(ipmiContext_,
                                     channel_number,
                                     deviceAccessAddress,
                                     lun,
                                     net_fn,
                                     (void const *) buf_rq, buf_rq_size,
                                     buf_rs, buf_rs_size);

  for(int i = 3; i < 3+num_bytes_to_read;i++){
    header.push_back(buf_rs[i]);
  } 
  ReadHeader();

  
  int bytes_read = 0;
  while (bytes_read < total_bytes_used){    
    
    int raw_result = ipmi_cmd_raw_ipmb(ipmiContext_,
				       channel_number,
				       deviceAccessAddress,
				       lun,
				       net_fn,
				       (void const *) buf_rq, buf_rq_size,
				       buf_rs, buf_rs_size);

    
    /* bytes 3 through 3+num_bytes_to_read are the useful information in buf_rs*/
    for(int i = 3; i < 3+num_bytes_to_read; i++){
      data.push_back(buf_rs[i]);
    }


    if(starting_byte_ls == 248){
      starting_byte_ms++;
      buf_rq[3] = starting_byte_ms;
    }

    starting_byte_ls += num_bytes_to_read;
    buf_rq[2] =  starting_byte_ls;

    bytes_read += num_bytes_to_read;
  }


  // check if the apollo contains each section of info and read them if so
  if(internalUseStartingOffset){
    ReadInternalUse();
  }
  if(chassisInfoStartingOffset){
    ReadChassisInfo();
  }
  if(boardStartingOffset){
    ReadBoardArea();
  }
  if(productInfoStartingOffset){
    ReadProductInfo();
  }
  if(multiRecordStartingOffset){
    ReadMultiRecord();
  }
  
}






int ApolloReader::ReadInformationLength(ipmi_ctx_t ipmiContext) {
  
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

  int raw_result = ipmi_cmd_raw_ipmb(ipmiContext,
				     channel_number,
				     deviceAccessAddress,
				     lun,
				     net_fn,
				     (void const *) buf_rq, buf_rq_size,
			             buf_rs, buf_rs_size);

  uint8_t length_ls_byte = buf_rs[2];
  uint8_t length_ms_byte = buf_rs[3];

  informationLength = 256*length_ms_byte + length_ls_byte;
  printf("information length: %d\n", informationLength);
  
  for(int i = 0; i < 5; i++){
    printf("LENGTH: %d ", buf_rs[i]);
  }
  printf("\n");
}


void ApolloReader::ReadHeader(){
  // read the first 8 bytes of data, figure out offsets, how many bytes are used
  for(int i = 0; i < header.size(); i++){
    printf("header[%d] = %d\n", i, header[i]);
  }
  headerFormatVersion = header[0];
  internalUseStartingOffset = header[1]*8;
  chassisInfoStartingOffset = header[2]*8;
  boardStartingOffset = header[3]*8;
  productInfoStartingOffset = header[4]*8;
  multiRecordStartingOffset = header[5]*8;
  pad = header[6];
  commonHeaderChecksum = header[7];
  total_bytes_used = informationLength - commonHeaderChecksum;
  printf("bytes used: %d\n", total_bytes_used);
}


void ApolloReader::ReadInternalUse(){
  int lenInternalUse = data[internalUseStartingOffset+1]*8;
  std::vector<uint8_t>::const_iterator first = data.begin() + internalUseStartingOffset;
  std::vector<uint8_t>::const_iterator last = data.begin() + internalUseStartingOffset + lenInternalUse;
  internalUseData = std::vector<uint8_t>(first, last);
  for (int i = 0; i < internalUseData.size(); i++){                                            
    if(i % 8 == 0){                                                                      
      printf("\n");                                                                      
    }                                                                                    
    printf("%02x ", internalUseData[i]);                                                       
  }                                                                                      
  printf("\n");
}     


void ApolloReader::ReadChassisInfo(){
  printf("chassisInfoStartingOffset %d\n", chassisInfoStartingOffset);
  int lenChassis = data[chassisInfoStartingOffset+1]*8;
  std::vector<uint8_t>::const_iterator first = data.begin() + chassisInfoStartingOffset;
  std::vector<uint8_t>::const_iterator last = data.begin() + chassisInfoStartingOffset + lenChassis;
  chassisData = std::vector<uint8_t>(first, last);
  for (int i = 0; i < chassisData.size(); i++){                                            
    if(i % 8 == 0){                                                                      
      printf("\n");                                                                      
    }                                                                                    
    printf("%02x ", chassisData[i]);                                                       
  }                                                                                      
  printf("\n");

}

void ApolloReader::ReadBoardArea(){
  int lenBoardArea = data[boardStartingOffset+1]*8;  
  std::vector<uint8_t>::const_iterator first = data.begin() + boardStartingOffset;
  std::vector<uint8_t>::const_iterator last = data.begin() + boardStartingOffset + lenBoardArea;
  boardData =  std::vector<uint8_t>(first, last);
  for (int i = 0; i < boardData.size(); i++){
    if(i % 8 == 0){
      printf("\n");
    }
    printf("%02x ", boardData[i]);
  }
  printf("\n");
}

void ApolloReader::ReadProductInfo(){
  printf("productInfoStartingOffset %d\n", productInfoStartingOffset);
  int lenProductInfo = data[productInfoStartingOffset+1]*8;
  std::vector<uint8_t>::const_iterator first = data.begin() + productInfoStartingOffset;
  std::vector<uint8_t>::const_iterator last = data.begin() + productInfoStartingOffset + lenProductInfo;
  productInfoData =  std::vector<uint8_t>(first, last);
  for (int i = 0; i < productInfoData.size(); i++){
    if(i % 8 == 0){
      printf("\n");
    }
    printf("%02x ", productInfoData[i]);
  }
  printf("\n");
}

void ApolloReader::ReadMultiRecord(){
  printf("multiRecordStartingOffset %d\n", multiRecordStartingOffset);
}


std::vector<uint8_t> ApolloReader::GetHeader(){
  return header;
}


std::vector<uint8_t> ApolloReader::GetData(){
  return data;
}
