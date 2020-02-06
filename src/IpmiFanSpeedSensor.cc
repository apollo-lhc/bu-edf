#include <iostream>
#include <string>
#include <freeipmi/api/ipmi-api.h>
#include <freeipmi/spec/ipmi-authentication-type-spec.h>
#include <freeipmi/spec/ipmi-privilege-level-spec.h>
#include <IpmiFanSpeedSensor.hh>

IpmiFanSpeedSensor::IpmiFanSpeedSensor(int sensorNum, std::string dbName){
  SetDatabaseName(dbName);
  SetSensorNumber(sensorNum);
}

void IpmiFanSpeedSensor::SetSensorNumber(int sensorNum){
  sensorNumber = sensorNum;
}

float IpmiFanSpeedSensor::GetVal(){
  ipmi_ctx_t ipmiContext = ipmi_ctx_create();
  const char *hostname = "192.168.10.171";

  int connection = ipmi_ctx_open_outofband(ipmiContext,
					   hostname,
					   "",
					   "",
					   IPMI_AUTHENTICATION_TYPE_NONE,
					   IPMI_PRIVILEGE_LEVEL_ADMIN,
					   0,0,0,0);

  if (connection < 0) {
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
  // sensor number (240-243 for fan speeds)
  buf_rq[1] = sensorNumber;

  const size_t buf_rs_size = 256;
  uint8_t buf_rs[buf_rs_size];
  int raw_result = ipmi_cmd_raw_ipmb ( ipmiContext,
				       channel_number,
				       rs_addr,
				       lun,
				       net_fn,
				       (void const *) buf_rq, buf_rq_size,
				       buf_rs, buf_rs_size );

  float raw_fan_speed = float(buf_rs[2]);
  float fan_speed_rpm = raw_fan_speed*46;
  int fan_speed_percent = raw_fan_speed/1.46;
  return fan_speed_percent;


}
