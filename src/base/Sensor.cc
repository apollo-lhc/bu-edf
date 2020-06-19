#include <base/Sensor.hh>
#include <base/net_helpers.hh>

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <string>

#include <arpa/inet.h>
#include <sys/types.h>

#include <unistd.h>

#include <syslog.h>

Sensor::Sensor(){
  sockfd = -1;
  bzero(&servaddr, sizeof(servaddr));
}

Sensor::~Sensor(){
  Disconnect();
}

int Sensor::Connect(std::string const &IP_addr, int port_number) {
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return -1;
  }

  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port_number); /* 2003 is plaintext port */

  if (inet_pton(AF_INET, IP_addr.c_str(), &servaddr.sin_addr) <= 0) {
      printf("inet_pton error for localhost");
      close(sockfd);
      sockfd = -1;
      return -1;
  }

  if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0){
    syslog(LOG_INFO,"connect error");
    close(sockfd);
    sockfd = -1;
    return -1;
  }
  return sockfd;
      
}
    
int Sensor::Disconnect() {
  if (sockfd == -1){
    return 0;
  }
  if (shutdown(sockfd, SHUT_RDWR) < 0){
    printf("shutdown failed");
    sockfd = -1;
    return -1;
  }
  sockfd = -1;
  return 0;
    
}
  
int Sensor::GetSockfd(){
  return sockfd;
}

void Sensor::SetDatabaseName(std::string const &name){
  databaseName = name;
}
  
int Sensor::Report(){
  /*CHECK IF databaseName.c_str() is empty (check std::string docs) ,return -1 if so*/
  if (databaseName.empty()){
    syslog(LOG_INFO,"database name is empty! Please update it before calling this function");
    return -1;
  }

  /* get float value for our particular sensor */
  float value = GetVal();

  /* get current time */
  time_t time_now = time(NULL);

  /* create our string to write to our whisper file */
  const size_t stringToWriteLen = 200;
  char *stringToWrite = new char[stringToWriteLen+1];
  memset(stringToWrite, 0, stringToWriteLen+1);

  /* make into one string */ /* make this database string*/
  snprintf(stringToWrite, stringToWriteLen, "%s %f %ld\n", databaseName.c_str(), value, time_now);

  /* write the result */
  if (writen(sockfd, stringToWrite, strlen(stringToWrite)) < 0) {
    printf("writen failed");
    if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0){
      printf("connect error");
      close(sockfd);
      sockfd = -1;
      return -1;
    }
    writen(sockfd, stringToWrite, strlen(stringToWrite));
    delete [] stringToWrite;
    return -1;
  }

  delete [] stringToWrite;

  return 0;
}
