#ifndef _SERVER_FUNCTIONS_H_
#define _SERVER_FUNCTIONS_H_
  void newUser(threadArgs_t threadArgs){
    (*threadArgs.totalConnected)++;
    threadArgs.usernames = (char**)realloc(threadArgs.usernames, sizeof(char*) * (*threadArgs.totalConnected));
    threadArgs.usernames[*threadArgs.totalConnected-1] = (char*)malloc(sizeof(char) * (MAX_USERNAME_LENGTH + 1));
  }
#endif
