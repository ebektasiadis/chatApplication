#ifndef _STRUCTS_H_
#define _STRUCTS_H_

  //pthread on create struct variable
  typedef struct {
    pthread_t tID;
    int* totalConnected;
    char** usernames;
  } threadArgs_t;

  typedef struct {
    void* dClient;
    void* dServer;
    threadArgs_t* threadArgs;
  } data_t;

  typedef struct {
    size_t lClient;
    size_t lServer;
    void (*function)(data_t);
  } router_t;

  //client structs
  typedef struct {
    char[MAX_USERNAME_LENGTH] username;
  } C_newUser();

  //server structs
  typedef struct {

  } S_newUser();



#endif
