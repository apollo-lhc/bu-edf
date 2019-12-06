#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string>

class Sensor {

public:
  Sensor();

  int Connect(std::string const &IP_addr, int port_number);

  int Disconnect();

  int GetSockfd();

  void SetDatabaseName(std::string const &name);

  int Report();

  virtual float GetVal();

private:
  Sensor( const Sensor& other );// = 0;
  Sensor( Sensor& other );// = 0;

  //  Sensor& operator=( const Sensor& rhs );
  //Sensor& operator=( Sensor& rhs );
  //  Sensor& operator=( Sensor rhs );
  //  const Sensor& operator=( const Sensor& rhs );
  //  const Sensor& operator=( Sensor& rhs );
  //  const Sensor& operator=( Sensor rhs );
  //  Sensor operator=( const Sensor& rhs );
  //  Sensor operator=( Sensor& rhs );
  //  Sensor operator=( Sensor rhs );

  int sockfd;
  
  struct sockaddr_in servaddr;

  std::string databaseName;
};
