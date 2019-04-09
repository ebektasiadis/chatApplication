#ifndef _STRUCTS_H_
#define _STRUCTS_H_


  //pthread on create struct variable

  typedef struct {
    int     totalMentions;
    char**  mentions;
    char    sender[MAX_USERNAME_LENGTH];
    char    message[MAX_MESSAGE_LENGTH];
  } messageIndex_t;

  typedef struct {
    int             totalMessages;
    messageIndex_t* messagesIndex;
  } messages_t;

  typedef struct {
    pthread_t   tID;
    int         sID;
    int*        totalConnected;
    char        username[MAX_USERNAME_LENGTH];
    char***     usernames;
    messages_t* messages;
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

  typedef struct {
    int pos;
  } C_getUser;

  typedef struct {
    char message[MAX_MESSAGE_LENGTH];
  } C_postMessage;

  //server structs
  typedef struct {
    int confirm;
  } S_newUser;

  typedef struct {
    int totalUsers;
  } S_getTotalUsers;

  typedef struct {
    char username[MAX_USERNAME_LENGTH];
  } S_getUser;

  typedef struct {
    messageIndex_t messages[MAX_RETURN_MESSAGES];
  } S_getMessages;

#endif
