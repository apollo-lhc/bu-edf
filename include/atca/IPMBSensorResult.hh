#ifndef __IPMB_SENSOR_RESULT_HH__
#define __IPMB_SENSOR_RESULT_HH__
#include <stdint.h>


struct IPMBSensorResult {
  uint8_t empty_byte1;
  uint8_t empty_byte22;
  uint8_t sensorValue;
  uint8_t status;
  uint8_t mask;
};
#endif
