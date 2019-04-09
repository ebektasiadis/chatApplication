#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
// #include <fcntl.h>
#include <unistd.h>
// #include <sys/types.h>
#include <arpa/inet.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netinet/ip.h>
#include "defines.h"
#include "structs.h"
#include "serverFunctions.h"

router_t indexes[] = {
  {NEW_USER, sizeof(C_newUser), sizeof(S_newUser), newUser},
  {GET_TOTAL_USERS, 0, sizeof(S_getTotalUsers), getTotalUsers},
  {GET_USER, sizeof(C_getUser), sizeof(S_getUser), getUser},
};

void* core(void* args){
  threadArgs_t threadArgs = *(threadArgs_t*)args;

  while(true){
    int cmd;
    void* input;
    data_t data;

    read(threadArgs.sID, &cmd, sizeof(int));
    printf("Received cmd: %d\n", cmd);
    if(cmd >= 0){
      printf("For this command you will receive %zu bytes and send back %zu bytes.\n", indexes[cmd].lClient, indexes[cmd].lServer);
      input = (void*)malloc(indexes[cmd].lClient);
      if(indexes[cmd].lClient > 0){
        printf("Read from client %lu bytes\n", read(threadArgs.sID, input, indexes[cmd].lClient));
        data.dClient = input;
      }

      if(indexes[cmd].lServer > 0){
        data.threadArgs = &threadArgs;
        data.dServer = (void*)malloc(indexes[cmd].lServer);
        indexes[cmd].function(&data);
        printf("Wrote on client %lu bytes\n", write(threadArgs.sID, data.dServer, indexes[cmd].lServer));
      }

    }else{
      switch(cmd){
        case DISCONNECT: pthread_exit(NULL); break;
      }
    }
  }
}

int main(int argc, char* argv[]){
  int totalConnected = 0;

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

  char** usernames = (char**)malloc(sizeof(char*));

  //Threading.
  while(true){
    clientv = accept(socketv, NULL , NULL);
    if(clientv < 1){
      perror("accept");
    }

    printf("New user logged in and found username pointer at %p\n", &usernames);

    if(pthread_create(&threadArgs.tID, NULL, core, &threadArgs) != 0){
      perror("pthread_create");
    }else{
      threadArgs.sID = clientv;
      threadArgs.usernames = &usernames;
      threadArgs.totalConnected = &totalConnected;
    }

    printf("User has been connected\n");
  }

  return 0;
}
