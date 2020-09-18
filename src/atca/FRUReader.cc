#include <atca/FRUReader.hh>
#include <iostream>
#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>
#include <stdexcept>
#include <string.h>
#include <string>

FRUReader::FRUReader(std::string const & hostname_, uint8_t deviceAddr, int fru_id_)
{

  hostname = hostname_;
  deviceAccessAddress = deviceAddr;
  fru_id = fru_id_;
  Read();
 
}

void FRUReader::PrintFRUInfo(bool verbose){
  if(!verbose){
    printf(" 0x%02x(%02d)", deviceAccessAddress, fru_id);
    if(productName != ""){
      printf(" | %*s", 60, productName.c_str());
    } else {
      printf(" | %*s", 60, "Unknown");
    }

    if(productSerial != ""){
      printf(" | %*s", 10, productSerial.c_str());
    } else {
      printf(" | %*s", 10,"N/A");
    }
  }
  printf("\n");
}

void FRUReader::Read(){

  ipmi_ctx_t ipmiContext_ = ipmi_ctx_create();


   int connection = ipmi_ctx_open_outofband(ipmiContext_,
					    hostname.c_str(),
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
  //  uint8_t fru_id = 0;
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
  int raw_result_header = ipmi_cmd_raw_ipmb(ipmiContext_,
                                     channel_number,
                                     deviceAccessAddress,
                                     lun,
                                     net_fn,
                                     (void const *) buf_rq, buf_rq_size,
                                     buf_rs, buf_rs_size);

  
  if (raw_result_header < 0){
    ipmi_ctx_close(ipmiContext_);
    throw std::runtime_error(strerror(ipmi_ctx_errnum(ipmiContext_)));  
  }


  for(int i = 3; i < 3+num_bytes_to_read;i++){
    header.push_back(buf_rs[i]);
  }

  ReadHeader();

  // max we can read is 22
  num_bytes_to_read = 22;
  buf_rq[4] = num_bytes_to_read;

  int bytes_read = 0;
  while (bytes_read < total_bytes_used){    

    int raw_result = ipmi_cmd_raw_ipmb(ipmiContext_,
				       channel_number,
				       deviceAccessAddress,
				       lun,
				       net_fn,
				       (void const *) buf_rq, buf_rq_size,
				       buf_rs, buf_rs_size);
    
    if (raw_result < 0){
      throw std::runtime_error(strerror(ipmi_ctx_errnum(ipmiContext_)));
    }
    
    /* bytes 3 through 3+num_bytes_to_read are the useful information in buf_rs*/
    for(int i = 3; i < 3+num_bytes_to_read; i++){
      data.push_back(buf_rs[i]);
    }


    if((starting_byte_ls + num_bytes_to_read) > 255 ){
      starting_byte_ms++;
      buf_rq[3] = starting_byte_ms;
    }

    starting_byte_ls += num_bytes_to_read;
    buf_rq[2] =  starting_byte_ls;

    bytes_read += num_bytes_to_read;

  }

  ipmi_ctx_close(ipmiContext_);
  
  // check if the apollo contains each section of info and read them if so
  /*  if(internalUseStartingOffset){
    ReadInternalUse();
    }*/
  
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






int FRUReader::ReadInformationLength(ipmi_ctx_t ipmiContext) {
  
  uint8_t channel_number = 0;
  uint8_t lun = 0;
  uint8_t net_fn = 0xa; // 0xa is net fn for storage
  const size_t buf_rq_size = 2;

  uint8_t buf_rq[buf_rq_size];

  uint8_t read_code = 0x10;
  //  uint8_t fru_id = 0;

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
  if (raw_result < 0){
    ipmi_ctx_close(ipmiContext);
    throw std::runtime_error(strerror(ipmi_ctx_errnum(ipmiContext)));  
  }

  uint8_t length_ls_byte = buf_rs[2];
  uint8_t length_ms_byte = buf_rs[3];

  informationLength = 256*length_ms_byte + length_ls_byte;
  return 0;
}


void FRUReader::ReadHeader(){
  // read the first 8 bytes of data, figure out offsets for each section
  headerFormatVersion = header[0];
  internalUseStartingOffset = header[1]*8;
  chassisInfoStartingOffset = header[2]*8;
  boardStartingOffset = header[3]*8;
  productInfoStartingOffset = header[4]*8;
  multiRecordStartingOffset = header[5]*8;
  pad = header[6];
  commonHeaderChecksum = header[7];
  total_bytes_used = informationLength;
}


void FRUReader::ReadInternalUse(){
  int lenInternalUse = data[internalUseStartingOffset+1]*8;
  std::vector<uint8_t>::const_iterator first = data.begin() + internalUseStartingOffset;
  std::vector<uint8_t>::const_iterator last = data.begin() + internalUseStartingOffset + lenInternalUse;
  internalUseData = std::vector<uint8_t>(first, last);
  for (size_t i = 0; i < internalUseData.size(); i++){                                            
    if(i % 8 == 0){                                                                      
      printf("\n");                                                                      
    }                                                                                    
    printf("%02x ", internalUseData[i]);                                                       
  }                                                                                      
  printf("\n");
}     





void FRUReader::ReadChassisInfo(){
  // chassis data currently is not read out
  // in order to do so, the same prrocess as in ReadBoardArea() and ReadProductInfo() should be followed
  int lenChassis = data[chassisInfoStartingOffset+1]*8;
  std::vector<uint8_t>::const_iterator first = data.begin() + chassisInfoStartingOffset;
  std::vector<uint8_t>::const_iterator last = data.begin() + chassisInfoStartingOffset + lenChassis;
  chassisData = std::vector<uint8_t>(first, last);

}




void FRUReader::ReadBoardArea(){
  // this field tells how long the data section is 
  int lenBoardArea = data[boardStartingOffset+1]*8;  
  std::vector<uint8_t>::const_iterator first = data.begin() + boardStartingOffset;
  std::vector<uint8_t>::const_iterator last = data.begin() + boardStartingOffset + lenBoardArea;
  // make a vector that just consists of the board information 
  boardData =  std::vector<uint8_t>(first, last);

  // the first 6 bytes are taken up by (0)format version
  //                                   (1)board area length
  //                                   (2)language code
  //                                   (3-5)manufacture time/date
  uint8_t fields_index = 6;


  bool last_field_reached = false;
  // perform the following for each field:
  // BOARD MANUFACTURER
  uint8_t board_manufacturer_type_and_length = boardData[fields_index];
  uint8_t board_man_index = fields_index+1;
  fields_index += ((board_manufacturer_type_and_length) & 0x3f)+1;
  // only lower 6 bits are is length of field,  upper 2 bits are 'type'
  if(!last_field_reached){
    boardManufacturer = ReadBoardField(board_manufacturer_type_and_length, board_man_index);
  } else {
    boardManufacturer = "";
  }
  if(board_manufacturer_type_and_length == 0xc1){
    last_field_reached = true;
    printf("BOARD MANUFACTURER IS END OF FIELDS\n");
  }

  // END BOARD MANUFACTURER

  // BOARD PRODUCT NAME
  uint8_t board_name_type_and_length = boardData[fields_index];
  uint8_t board_name_index = fields_index+1;
  fields_index += ((board_name_type_and_length) & 0x3f)+1;
  if(!last_field_reached){
    boardName = ReadBoardField(board_name_type_and_length, board_name_index);
  } else {
    boardName = "";
  }
  if(board_name_type_and_length == 0xc1){
    last_field_reached = true;
    printf("BOARD PRODUCT NAME IS END OF FIELDS\n");
  }
  // END BOARD NAME
  
  // BOARD SERIAL NUMBER
  uint8_t board_serial_type_and_length = boardData[fields_index];
  uint8_t board_serial_index = fields_index+1;
  fields_index += ((board_serial_type_and_length) & 0x3f)+1;
  if(!last_field_reached){
    boardSerial = ReadBoardField(board_serial_type_and_length, board_serial_index);
  } else{
    boardSerial = "";
  }
  if(board_serial_type_and_length == 0xc1){
    last_field_reached = true;
    printf("BOARD SERIAL IS END OF FIELDS\n");
  }
  // END SERIAL NUMBER

  // BOARD PART NUMBER
  uint8_t board_part_type_and_length = boardData[fields_index];
  uint8_t board_part_index = fields_index+1;
  fields_index += ((board_part_type_and_length) & 0x3f)+1;
  if(!last_field_reached){
    boardPartNumber = ReadBoardField(board_part_type_and_length, board_part_index);
  } else {
    boardPartNumber = "";
  }
  if(board_part_type_and_length == 0xc1){
    last_field_reached = true;
    printf("BOARD PART NUMBER IS END OF FIELDS\n");
  }
  // END PART NUMBER

  // FRU FILE ID
  uint8_t fru_file_id_type_and_length = boardData[fields_index];
  uint8_t fru_file_id_index = fields_index+1;
  fields_index += ((fru_file_id_type_and_length) & 0x3f)+1;
  if(!last_field_reached){
    fruFileId = ReadBoardField(fru_file_id_type_and_length, fru_file_id_index);
  } else {
    fruFileId = "";
  }
  if(fru_file_id_type_and_length == 0xc1){
    last_field_reached = true;
    printf("FRU FILE ID IS END OF FIELDS\n");
  }
  // END FRU FILE ID  
}
std::string FRUReader::ReadBoardField(uint8_t field_type_and_length, uint8_t field_index){
  uint8_t field_length = ((field_type_and_length) & 0x3f);
  //uint8_t field_type = ((field_type_and_length) & 0xc0); //Check in future...
  std::string field = "";
  for(int i = 0; i < field_length; i++){
    field += boardData[field_index+i];
  }
  return field;
}

std::string FRUReader::GetBoardManufacturer(){
  return boardManufacturer;
}
std::string FRUReader::GetBoardName(){
  return boardName;
}
std::string FRUReader::GetBoardSerial(){
  return boardSerial;
}
std::string FRUReader::GetBoardPartNumber(){
  return boardPartNumber;
}
std::string FRUReader::GetFruFileId(){
  return fruFileId;
}







void FRUReader::ReadProductInfo(){
  //  printf("product info:");
  int lenProductInfo = data[productInfoStartingOffset+1]*8;
  std::vector<uint8_t>::const_iterator first = data.begin() + productInfoStartingOffset;
  std::vector<uint8_t>::const_iterator last = data.begin() + productInfoStartingOffset + lenProductInfo;
  productInfoData =  std::vector<uint8_t>(first, last);
  /*for (int i = 0; i < productInfoData.size(); i++){
    if(i % 8 == 0){
      printf("\n");
    }
    printf("%02x ", productInfoData[i]);
  }
  printf("\n");*/

  uint8_t fields_index = 3;
  bool last_field_reached = false;
  // PRODUCT MANUFACTURER
  uint8_t product_manufacturer_type_and_length = productInfoData[fields_index];
  uint8_t product_man_index = fields_index+1;
  fields_index += ((product_manufacturer_type_and_length) & 0x3f)+1;
  // only lower 6 bits are is length of field,  upper 2 bits are 'type'
  if(!last_field_reached){
    productManufacturer = ReadProductField(product_manufacturer_type_and_length, product_man_index);
  } else {
    productManufacturer = "";
  }
  if(product_manufacturer_type_and_length == 0xc1){
    last_field_reached = true;
    printf("PRODUCT MANUFACTURER IS END OF FIELDS\n");
  }
  // END PRODUCT MANUFACTURER

  // PRODUCT NAME
  uint8_t product_name_type_and_length = productInfoData[fields_index];
  uint8_t product_name_index = fields_index+1;
  fields_index += ((product_name_type_and_length) & 0x3f)+1;
  // only lower 6 bits are is length of field,  upper 2 bits are 'type'
  if(!last_field_reached){
    productName = ReadProductField(product_name_type_and_length, product_name_index);
  } else {
    productName = "";
  }
  if(product_name_type_and_length == 0xc1){
    last_field_reached = true;
    printf("PRODUCT NAME IS END OF FIELDS\n");
  }
  //  printf("PRODUCT NAME IS: %s\n", productName.c_str());
  // END PRODUCT NAME

  // PRODUCT PART NUMBER
  uint8_t product_num_type_and_length = productInfoData[fields_index];
  uint8_t product_num_index = fields_index+1;
  fields_index += ((product_num_type_and_length) & 0x3f)+1;
  // only lower 6 bits are is length of field,  upper 2 bits are 'type'
  if(product_num_type_and_length == 0xc1){
    last_field_reached = true;
    printf("PRODUCT PART # IS END OF FIELDS\n");
  }
  if(!last_field_reached){
    productPartNumber = ReadProductField(product_num_type_and_length, product_num_index);
  } else {
    productPartNumber = "";
  }
  // END PRODUCT PART NUMBER

  // PRODUCT VERSION
  uint8_t product_version_type_and_length = productInfoData[fields_index];
  uint8_t product_version_index = fields_index+1;
  fields_index += ((product_version_type_and_length) & 0x3f)+1;
  // only lower 6 bits are is length of field,  upper 2 bits are 'type'
  if(!last_field_reached){
    productVersion = ReadProductField(product_version_type_and_length, product_version_index);
  } else {
    productVersion = "";
  }
  if(product_version_type_and_length == 0xc1){
    last_field_reached = true;
    printf("PRODUCT VERSION IS END OF FIELDS\n");
  }
  // END PRODUCT VERSION

  // PRODUCT SERIAL
  uint8_t product_serial_type_and_length = productInfoData[fields_index];
  uint8_t product_serial_index = fields_index+1;
  fields_index += ((product_serial_type_and_length) & 0x3f)+1;
  // only lower 6 bits are is length of field,  upper 2 bits are 'type'
  if(!last_field_reached){
    productSerial = ReadProductField(product_serial_type_and_length, product_serial_index);
  } else {
    productSerial = "";
  }
  if(product_serial_type_and_length == 0xc1){
    last_field_reached = true;
    printf("PRODUCT SERIAL IS END OF FIELDS\n");
  }
  //  printf("PRODUCT SERIAL IS: %s\n", productSerial.c_str());
  // END PRODUCT SERIAL

  // ASSSET TAG
  uint8_t asset_tag_type_and_length = productInfoData[fields_index];
  uint8_t asset_tag_name_index = fields_index+1;
  fields_index += ((asset_tag_type_and_length) & 0x3f)+1;
  // only lower 6 bits are is length of field,  upper 2 bits are 'type'
  if(!last_field_reached){
    assetTag = ReadProductField(asset_tag_type_and_length, asset_tag_name_index);
  } else {
    last_field_reached = "";
  }
  if(asset_tag_type_and_length == 0xc1){
    last_field_reached = true;
    printf("ASSET TAG IS END OF FIELDS\n");
  // END ASSET TAG
  }
}
std::string FRUReader::ReadProductField(uint8_t field_type_and_length, uint8_t field_index){
  uint8_t field_length = ((field_type_and_length) & 0x3f);
  //  uint8_t field_type = ((field_type_and_length) & 0xc0); //check in future...
  std::string field = "";
  for(int i = 0; i < field_length; i++){
    field += productInfoData[field_index+i];
  }
  return field;
}
std::string FRUReader::GetProductManufacturer(){
  return productManufacturer;
}
std::string FRUReader::GetProductName(){
  return productName;
}
std::string FRUReader::GetProductPartNumber(){
  return productPartNumber;
}
std::string FRUReader::GetProductVersion(){
  return productVersion;
}
std::string FRUReader::GetProductSerial(){
  return productSerial;
}
std::string FRUReader::GetAssetTag(){
  return assetTag;
}


void FRUReader::ReadMultiRecord(){

}


std::vector<uint8_t> FRUReader::GetHeader(){
  return header;
}


std::vector<uint8_t> FRUReader::GetData(){
  return data;
}

int FRUReader::GetFruId(){
  return fru_id;
}
