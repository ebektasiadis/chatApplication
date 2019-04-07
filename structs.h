#ifndef _STRUCTS_H_
#define _STRUCTS_H_

  //pthread on create struct variable
  typedef struct {
    pthread_t tID;
    int       sID;
    int*      totalConnected;
    char**    usernames;
  } threadArgs_t;

  typedef struct {
    void*         dClient;
    void*         dServer;
    threadArgs_t* threadArgs;
  } data_t;

  typedef struct {
    int     cmd;
    size_t  lClient;
    size_t  lServer;
    void    (*function)(data_t*);
  } router_t;

  //client structs
  typedef struct {
    char username[MAX_USERNAME_LENGTH];
  } C_newUser;

  //server structs
  typedef struct {
    int confirm;
  } S_newUser;

#endif
