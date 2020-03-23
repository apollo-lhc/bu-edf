#ifndef __APOLLO__READER__HH__
#define __APOLLO__READER__HH__

#include <string>
#include <vector>

class ApolloReader {

public:

  ApolloReader(char *hostname_, uint8_t deviceAddr);
  
  std::vector<uint8_t> GetHeader();
  
  int GetProductSerial();

  std::string GetProductName();
  
private:

  void Read();

  char *hostname;
  
  uint8_t deviceAccessAddress;
  
  std::vector<uint8_t> header;
  
  std::vector<uint8_t> data;

};
#endif
