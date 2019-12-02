#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>


class Sensor {

public:
  Sensor();

  int connect(std::string const &IP_addr, int port_number);

  int disconnect();

  int GetSockfd();

  void SetDatabaseName(std::string const &name);

  int report();

  virtual float get_val();

private:
  Sensor( const Sensor& other ) = 0;
  Sensor( Sensor& other ) = 0;

  MyClass& operator=( const MyClass& rhs );
  MyClass& operator=( MyClass& rhs );
  MyClass& operator=( MyClass rhs );
  const MyClass& operator=( const MyClass& rhs );
  const MyClass& operator=( MyClass& rhs );
  const MyClass& operator=( MyClass rhs );
  MyClass operator=( const MyClass& rhs );
  MyClass operator=( MyClass& rhs );
  MyClass operator=( MyClass rhs );

  int sockfd;
  
  struct sockaddr_in servaddr;

  std::string databaseName;
}
