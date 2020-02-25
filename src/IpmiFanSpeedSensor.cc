#include <iostream>
#include <string>
#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>
#include <IpmiFanSpeedSensor.hh>
#include <stdexcept>
#include <IpmbSensorResult.hh>
#include <string.h>

IpmiFanSpeedSensor::IpmiFanSpeedSensor(int sensorNum, std::string dbName, char *hostname_, uint8_t deviceAddr){
  SetDatabaseName(dbName);
  SetSensorNumber(sensorNum);
  SetHostname(hostname_);
  SetDeviceAccessAddress(deviceAddr);
}

void IpmiFanSpeedSensor::SetSensorNumber(int sensorNum){
  sensorNumber = sensorNum;
}

void IpmiFanSpeedSensor::SetHostname(char *hostname_){
  hostname = hostname_;
}

void IpmiFanSpeedSensor::SetDeviceAccessAddress(uint8_t deviceAddr){
  deviceAccessAddress = deviceAddr;
}

float IpmiFanSpeedSensor::GetVal(){
  ipmi_ctx_t ipmiContext = ipmi_ctx_create();

  int connection = ipmi_ctx_open_outofband(ipmiContext,
					   hostname,
					   "",
					   "",
					   IPMI_AUTHENTICATION_TYPE_NONE,
					   IPMI_PRIVILEGE_LEVEL_ADMIN,
					   0,0,0,0);

  if (connection < 0) {
    throw std::runtime_error("Runtime error, returning -1");
    return -1;
  }

  
  uint8_t channel_number = 0;
  uint8_t lun = 0;
  // net_fn code for "Sensor Events" 0x04
  uint8_t net_fn = 0x04;

  // it's necessary to send 2 raw bytes to get our sensor data back
  const size_t buf_rq_size = 2;
  uint8_t buf_rq[buf_rq_size];
  // 0x2d is the ipmi raw command for reading sensor values
  buf_rq[0] = 0x2d;
  // sensor number (240-243 for fan speeds)
  buf_rq[1] = sensorNumber;

  const size_t buf_rs_size = 256;
  uint8_t buf_rs[buf_rs_size];
  int raw_result = ipmi_cmd_raw_ipmb ( ipmiContext,
				       channel_number,
				       deviceAccessAddress,
				       lun,
				       net_fn,
				       (void const *) buf_rq, buf_rq_size,
				       buf_rs, buf_rs_size );

  if(raw_result == -1 || raw_result == 0) {
    throw std::runtime_error(strerror(ipmi_ctx_errnum(ipmiContext)));
  }
  
  // close connection
  ipmi_ctx_close(ipmiContext);

  IpmbSensorResult *ipmbSensorVal = (IpmbSensorResult *) buf_rs;

  if ( ipmbSensorVal->status == 0x20 ) {
    throw std::runtime_error("Event messages are disabled from this fanspeed sensor\n");
  }

  if (ipmbSensorVal->sensorValue < 0 ){
    throw std::range_error("Invalid sensor value\n");
  }

  
  float raw_fan_speed = ipmbSensorVal->sensorValue;
  float fan_speed_rpm = raw_fan_speed*46;
  int fan_speed_percent = raw_fan_speed/1.46;
  return fan_speed_percent;
}
