#ifndef __SDR_READER__HH__
#define __SDR_READER__HH__

#include <string>
#include <vector>
#include <freeipmi/api/ipmi-api.h>

class SDRReader {
public:
  Read();
private:
  std::vector<uint8_t> data;
};
