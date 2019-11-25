#include "myunp.h"

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <string>

#include <arpa/inet.h>
#include <sys/types.h>

#define SA  struct sockaddr

class Sensor {

  int sockfd;
    
  struct sockaddr_in * servaddr;



  public:
  int connect(std::string IP_addr, int port_number) {
    if (sockfd = socket(AF_INET, SOCK_STREAM, 0) < ) {
      printf("sockfd < 0");
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(2003); /* 2003 is plaintext port */

    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
	printf("inet_pton error for localhost");
    }

    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0){
      printf("connect error");
    }
 
      
  }
    
  int disconnect() {
    if (shutdown(sockfd, SHUT_RDWR) < 0){
      printf("shutdown failed");
      return -1;
    }
    sockfd = -1;
    return 0;
    
  }
  
  int get_sockfd(){
    return sockfd;
  }
  
  int report(){

    /* get float value for our particular sensor */
    float value = get_val();

    /* get current time */
    time_t time_now = time(NULL);

    /* create our string to write to our whisper file */
    const size_t stringToWriteLen = 200;
    char *stringToWrite = new char[stringToWriteLen+1];
    memset(stringToWrite, 0, stringToWriteLen+1);

    /* make into one string */
    snprintf(stringToWrite, stringToWriteLen, "sensors.unix_time %f %d\n", value, time_now);

    /* write the result */
    if (writen(sockfd, stringToWrite, strlen(stringToWrite) < 0) {
	printf("writen failed");
	return -1;
    }
  }

  virtual float get_val();

}
