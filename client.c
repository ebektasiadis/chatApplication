#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "defines.h"
#include "structs.h"
#include "clientFunctions.h"

int socketv;
router_t indexes[] = {
  {NEW_USER, sizeof(C_newUser), sizeof(S_newUser)},
  {GET_TOTAL_USERS, 0, sizeof(S_getTotalUsers)},
  {GET_USER, sizeof(C_getUser), sizeof(S_getUser)},
  {POST_MESSAGE, sizeof(C_postMessage), 0},
  {GET_MESSAGES, 0, sizeof(S_getMessages)},
};

void sigHandler(int sigID){
  sendCmd(DISCONNECT, NULL);
  exit(1);
}


int main(int argc, char* argv[]){
  int connectv;
  struct sockaddr_in socketAddr;
  C_newUser cNewUser;

  signal(SIGINT, sigHandler);
  signal(SIGTSTP, sigHandler);

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

  cNewUser = usernameInput();

  char msg[MAX_MESSAGE_LENGTH], msgToBeTokenized[MAX_MESSAGE_LENGTH];;
  char* word;
  char** words = (char**)malloc(sizeof(char*));
  int totalWords;

  printf("Connected. Type /? or /help to view the available commands.\n");

  while(true){
    fgets(msg, sizeof(msg), stdin);
    msg[strlen(msg)-1] = '\0';
    totalWords = 0;
    system("clear");

    strcpy(msgToBeTokenized, msg);
    word = strtok(msgToBeTokenized, " ");

    while(word != NULL) {
      totalWords++;
      words = (char**)realloc(words, sizeof(char*)*totalWords);
      words[totalWords-1] = (char*)malloc(sizeof(char) * strlen(word));
      strcpy(words[totalWords-1], word);
      word = strtok(NULL, " ");
    }

    if(totalWords == 0)
      continue;

    if(!strcmp(words[0], "/u") || !strcmp(words[0], "/users")){
      showUsers();
    }else if(!strcmp(words[0], "/r") || !strcmp(words[0], "/refresh")){
      showMessages();
    }else if(!strcmp(words[0], "/e") || !strcmp(words[0], "/exit")){
      sendCmd(DISCONNECT, NULL);
      exit(0);
    }else if(!strcmp(words[0], "/?")  || !strcmp(words[0], "/help")){
      printf("Chat Application\n");
      printf("/u OR /users   :: Show online users\n");
      printf("/r OR /refresh :: Refresh messages\n");
      printf("/e OR /exit    :: Exit application (Can be happen with CTRL+C too)\n");
    }else if(strlen(words[0]) > 0 && words[0][0] == '@' && totalWords > 1){
      sendCmd(POST_MESSAGE, msg);
      showMessages();
    }else{
      printf("Commmand not found\n");
    }
    //FREE ARRAY
  }

  return 0;
}

void* sendCmd(int cmd, void* data){
  void* response;

  write(socketv, &cmd, sizeof(cmd));
  if(cmd >= 0){
    response = (void*)malloc(indexes[cmd].lServer);
    if(indexes[cmd].lClient > 0){
      write(socketv, data, indexes[cmd].lClient);
    }
    if(indexes[cmd].lServer > 0){
      read(socketv, response, indexes[cmd].lServer);
    }
  }

  return response;
}
