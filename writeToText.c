#include "myunp.h"

#include <string.h>
#include <time.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <sys/types.h>

#define SA  struct sockaddr


int main(int argc, char **argv){

  int sockfd;

  struct sockaddr_in servaddr;

  if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    printf("sockfd < 0");
  }

  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(2003); /* plaintext port */

  if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
    printf("inet_pton error for 127.0.0.1");
  }

  if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0){
    printf("connect error");
  }

  /* WRITE HERE */
  time_t t = time(NULL);

  const size_t stringToWriteLen = 200;
  char *stringToWrite = new char[stringToWriteLen+1];
  memset(stringToWrite, 0, stringToWriteLen+1);
  snprintf(stringToWrite, stringToWriteLen, 
	   "data.test 105 %d\n", t);

  printf("%s\n", stringToWrite);
  int writen_result = writen(sockfd, stringToWrite, strlen(stringToWrite));
  printf("writen_result %d\n", writen_result);
  shutdown(sockfd, SHUT_RDWR);
  
  return 0;

}
