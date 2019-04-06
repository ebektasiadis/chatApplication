#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <sys/types.h>
#include <arpa/inet.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netinet/ip.h>
#include "defines.h"
#include "structs.h"
#include "serverFunctions.h"


void* core(void* args){
  threadArgs_t threadArgs = *(threadArgs_t*)args;
  newUser(threadArgs);
  return NULL;
}

int main(int argc, char* argv[]){
  int totalConnected = 0;
  char** usernames;

  int bindv, socketv, clientv, threadv;
  struct sockaddr_in socketAddr;

  threadArgs_t threadArgs;

  socketv = socket(AF_INET, SOCK_STREAM, 0);

  if(socket<0){
    perror("socket");
    exit(1);
  }

  socketAddr.sin_family = AF_INET;
  socketAddr.sin_port = htons(atoi(argv[1]));
  socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  bindv = bind(socketv, (struct sockaddr *)&socketAddr, sizeof(socketAddr));

  if(bindv<0){
    perror("bind");
    exit(1);
  }

  if(listen(socketv, 1) < 0){
    perror("listen");
    exit(1);
  }

  usernames = (char**)malloc(sizeof(char*));

  //Threading.
  while(true){
    clientv = accept(socketv, NULL , NULL);
    if(clientv < 1){
      perror("accept");
    }


    if(pthread_create(&threadArgs.tID, NULL, core, &threadArgs) != 0){
      perror("pthread_create");
    }else{
      threadArgs.usernames = usernames;
      threadArgs.totalConnected = &totalConnected;
    }

    printf("User has been connected\n");
  }

  return 0;
}
