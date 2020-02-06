#include <stdio.h>
#include <iostream>
#include <string>
#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>
#include <IpmiTemperatureSensor.hh>

IpmiTemperatureSensor::IpmiTemperatureSensor(int sensorNum, std::string dbName){
  SetDatabaseName(dbName);
  SetSensorNumber(sensorNum);
}

void IpmiTemperatureSensor::SetSensorNumber(int sensorNum) {
  sensorNumber = sensorNum;
}

float IpmiTemperatureSensor::GetVal(){
  ipmi_ctx_t ipmiContext = ipmi_ctx_create();
  const char *hostname = "192.168.10.171";

  int connection = ipmi_ctx_open_outofband(ipmiContext,
					   hostname,
					   "",
					   "",
					   IPMI_AUTHENTICATION_TYPE_NONE,
					   IPMI_PRIVILEGE_LEVEL_ADMIN,
					   0,0,0,0);

  if ( connection < 0){
    return -1;
} 

  uint8_t channel_number = 0;
  // Device access address
  uint8_t rs_addr = 0x20;

  uint8_t lun = 0;
 // net_fn code for "Sensor Events" 0x04
  uint8_t net_fn = 0x04;

  // it's necessary to send 2 raw bytes to get our sensor data back
  const size_t buf_rq_size = 2;
  uint8_t buf_rq[buf_rq_size];
  // 0x2d is the ipmi raw command for reading sensor values
  buf_rq[0] = 0x2d;
  // sensor number (235-238 for tray temps, 240-243 for fan speeds, etc)
  buf_rq[1] = sensorNumber;

  const size_t buf_rs_size = 256;
  uint8_t buf_rs[buf_rs_size];
  int raw_result = ipmi_cmd_raw_ipmb ( ipmiContext,
				       channel_number,
				       rs_addr,
				       lun,
				       net_fn,
				       (void const *) buf_rq, buf_rq_size,
				       buf_rs, buf_rs_size);

  // the 3rd byte of output is the useful one
  return float(buf_rs[2]);
}
