#ifndef __APOLLO__READER__HH__
#define __APOLLO__READER__HH__

#include <string>
#include <vector>
#include <freeipmi/api/ipmi-api.h>

class ApolloReader {

public:

  ApolloReader(char *hostname_, uint8_t deviceAddr);
  
  std::vector<uint8_t> GetHeader();

  std::vector<uint8_t> GetData();

  int GetProductSerial();

  std::string GetProductName();

  void Read();
  
private:

  void ReadHeader();

  void ReadInternalUse();
  void ReadChassisInfo();
  void ReadBoardArea();
  void ReadProductInfo();
  void ReadMultiRecord();

  
  int ReadInformationLength(ipmi_ctx_t ipmiContext);

  int informationLength;
  int total_bytes_used;
  
  char *hostname;
  
  uint8_t deviceAccessAddress;
  
  std::vector<uint8_t> header;
  // fields for data stored in header
  uint8_t headerFormatVersion;
  uint8_t internalUseStartingOffset;
  uint8_t chassisInfoStartingOffset;
  uint8_t boardStartingOffset;
  uint8_t productInfoStartingOffset;
  uint8_t multiRecordStartingOffset;
  uint8_t pad;
  uint8_t commonHeaderChecksum;

  // raw hex data from the blade
  std::vector<uint8_t> data;
  // sub vectors for each info region
  std::vector<uint8_t> internalUseData;
  std::vector<uint8_t> chassisData;
  std::vector<uint8_t> boardData;
  std::vector<uint8_t> productInfoData;
  std::vector<uint8_t> multiRecordData;

};
#endif
