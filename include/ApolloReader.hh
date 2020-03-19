#ifndef __APOLLO__READER__HH__
#define __APOLLO__READER__HH__

class ApolloReader {

public:

  ApolloReader();
  
  std::vector<uint8_t> GetHeader();
  
  int GetProductSerial();

  std::string GetProductName();
  
private:

  void Read();
  
  std::vector<uint8_t> header;
  
  std::vector<uint8_t> data;

};
#endif
