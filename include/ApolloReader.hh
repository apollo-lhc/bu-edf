#ifndef __APOLLO__READER__HH__
#define __APOLLO__READER__HH__

#include <string>
#include <vector>
#include <freeipmi/api/ipmi-api.h>

class ApolloReader {

public:

  ApolloReader(char *hostname_, uint8_t deviceAddr);
  
  std::vector<uint8_t> GetHeader();
  
  int GetProductSerial();

  std::string GetProductName();

  void Read();
  
private:

  void ReadHeader(ipmi_ctx_t ipmiContext);
  
  int ReadInformationLength(ipmi_ctx_t ipmiContext);

  int informationLength;
  
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
  
  
  std::vector<uint8_t> data;

};
#endif
