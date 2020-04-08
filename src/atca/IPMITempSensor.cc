#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>
#include <atca/IPMITempSensor.hh>
#include <stdexcept>
#include <string.h>
#include <atca/IPMBSensorResult.hh>

IPMITempSensor::IPMITempSensor(int sensorNum, std::string dbName, char *hostname_, uint8_t deviceAddr){
  SetDatabaseName(dbName);
  SetSensorNumber(sensorNum);
  SetHostname(hostname_);
  SetDeviceAccessAddress(deviceAddr);
}

void IPMITempSensor::SetSensorNumber(int sensorNum) {
  sensorNumber = sensorNum;
}

void IPMITempSensor::SetHostname(char *hostname_){
  hostname = hostname_;
}

void IPMITempSensor::SetDeviceAccessAddress(uint8_t deviceAddr){
  deviceAccessAddress = deviceAddr;
}


float IPMITempSensor::GetVal(){

  // we must make this code non-blocking!
  ipmi_ctx_t ipmiContext = ipmi_ctx_create();


  int connection = ipmi_ctx_open_outofband(ipmiContext,
					   hostname,
					   "",
					   "",
					   IPMI_AUTHENTICATION_TYPE_NONE,
					   IPMI_PRIVILEGE_LEVEL_ADMIN,
					   0,0,0,0);

  if ( connection < 0){
    // THROW CONNECTION HERE!    std::
    throw std::runtime_error(strerror(ipmi_ctx_errnum(ipmiContext)));
    return -1;
  } 

  uint8_t channel_number = 0;
  // Device access address
  //  uint8_t rs_addr = 0x20;

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
				       deviceAccessAddress,
				       lun,
				       net_fn,
				       (void const *) buf_rq, buf_rq_size,
				       buf_rs, buf_rs_size);

  
  
  if(raw_result == -1 || raw_result == 0) {
    throw std::runtime_error(strerror(ipmi_ctx_errnum(ipmiContext)));
  }

  ipmi_ctx_close(ipmiContext);

  
  IPMBSensorResult *ipmbSensorVal = (IPMBSensorResult *) buf_rs;
  
  
  
  if( ipmbSensorVal->status != 0xc0 ) {
    throw std::runtime_error("Event messages are disabled from this sensor\n");
  }
  
  if ( ipmbSensorVal->sensorValue < 0 ){
    throw std::range_error("Invalid sensor value\n");
  }

  // the 3rd byte of output is the useful one
  return float(ipmbSensorVal->sensorValue);
}
