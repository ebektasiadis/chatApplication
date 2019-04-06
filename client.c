#include <stdio.h>
#include <stdlib.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <sys/types.h>
#include <arpa/inet.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netinet/ip.h>
#include "defines.h"
#include "structs.h"

int main(int argc, char* argv[]){
  int connectv, socketv;
  struct sockaddr_in socketAddr;

  socketv = socket(AF_INET, SOCK_STREAM, 0);

  if(socket<0){
      perror("socket");
      exit(1);
  }

  socketAddr.sin_family = AF_INET;
  socketAddr.sin_port = htons(atoi(argv[1]));
  socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  connectv = connect(socketv, (struct sockaddr *)&socketAddr, sizeof(socketAddr));

  if(connectv<0){
      perror("connect");
      exit(1);
  }

  return 0;
}
