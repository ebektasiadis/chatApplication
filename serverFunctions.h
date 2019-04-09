#ifndef _SERVER_FUNCTIONS_H_
#define _SERVER_FUNCTIONS_H_

void newUser(data_t* data){
  C_newUser cNewUser = *(C_newUser*)data->dClient;
  S_newUser* sNewUser;
  sNewUser = data->dServer;

  if(strlen(cNewUser.username) > MAX_USERNAME_LENGTH){
    sNewUser->confirm = 1;
    return;
  }

  int i;
  for(i = 0; i < *data->threadArgs->totalConnected; i++){
    if(strcmp(cNewUser.username, (*data->threadArgs->usernames)[i]) == 0){
      sNewUser->confirm = 2;
      return;
    }
  }

  if(strlen(cNewUser.username) == 0){
    sNewUser->confirm = 3;
    return;
  }

  for(i = 0; i < strlen(cNewUser.username); i++){
    switch(cNewUser.username[i]){
      case '\\':
      case ' ':
      case '/':
      case '@': sNewUser->confirm = 4;
                return;
                break;
    }
  }

  (*data->threadArgs->totalConnected)++;
  *data->threadArgs->usernames = (char**)realloc(*data->threadArgs->usernames, sizeof(char*) * (*data->threadArgs->totalConnected));
  (*data->threadArgs->usernames)[*data->threadArgs->totalConnected-1] = (char*)malloc(sizeof(char) * (MAX_USERNAME_LENGTH));
  strcpy((*data->threadArgs->usernames)[(*data->threadArgs->totalConnected)-1], cNewUser.username);
  strcpy(data->threadArgs->username, cNewUser.username);
  sNewUser->confirm = 0;
  return;
}

void getTotalUsers(data_t* data){
  S_getTotalUsers* sGetTotalUsers;
  sGetTotalUsers = data->dServer;

  sGetTotalUsers->totalUsers = *data->threadArgs->totalConnected;
  return;
}

void getUser(data_t* data){
  C_getUser cGetUser = *(C_getUser*)data->dClient;
  S_getUser* sGetUser;
  sGetUser = data->dServer;

  strcpy(sGetUser->username, (*data->threadArgs->usernames)[cGetUser.pos]);
  return;
}

void postMessage(data_t* data){
  int i, curMessage;
  char* word;
  char messageToBeTokenized[MAX_MESSAGE_LENGTH];
  C_postMessage cPostMessage = *(C_postMessage*)data->dClient;

  data->threadArgs->messages->totalMessages++;
  curMessage = data->threadArgs->messages->totalMessages-1;
  data->threadArgs->messages->messagesIndex = (messageIndex_t*)realloc(data->threadArgs->messages->messagesIndex, sizeof(messageIndex_t) * (data->threadArgs->messages->totalMessages));
  strcpy(data->threadArgs->messages->messagesIndex[curMessage].sender, data->threadArgs->username);
  strcpy(data->threadArgs->messages->messagesIndex[curMessage].message, cPostMessage.message);
  printf("%s sent: %s\n", data->threadArgs->messages->messagesIndex[curMessage].sender, data->threadArgs->messages->messagesIndex[curMessage].message);
  data->threadArgs->messages->messagesIndex[curMessage].totalMentions = 0;

  strcpy(messageToBeTokenized, data->threadArgs->messages->messagesIndex[curMessage].message);
  word = strtok(messageToBeTokenized, " ");

  while(word != NULL) {
    if(strlen(word) > 0 && strlen(word) <= MAX_USERNAME_LENGTH+1 && word[0] == '@'){
      data->threadArgs->messages->messagesIndex[curMessage].totalMentions++;
      data->threadArgs->messages->messagesIndex[curMessage].mentions = (char**)realloc(data->threadArgs->messages->messagesIndex[curMessage].mentions, sizeof(char*) * data->threadArgs->messages->messagesIndex[curMessage].totalMentions);
      data->threadArgs->messages->messagesIndex[curMessage].mentions[data->threadArgs->messages->messagesIndex[curMessage].totalMentions-1] = (char*)malloc(MAX_USERNAME_LENGTH * sizeof(char));
      for(i = 1; i < strlen(word); i++){
        data->threadArgs->messages->messagesIndex[curMessage].mentions[data->threadArgs->messages->messagesIndex[curMessage].totalMentions-1][i-1] = word[i];
      }
      data->threadArgs->messages->messagesIndex[curMessage].mentions[data->threadArgs->messages->messagesIndex[curMessage].totalMentions-1][i-1] = '\0';
    }
    word = strtok(NULL, " ");
  }
  return;
}

void getMessages(data_t* data){
  S_getMessages* sGetMessages;
  sGetMessages = data->dServer;
  int totalMessages = 0;

  int i,j;
  for(i = data->threadArgs->messages->totalMessages-1; i >= 0 ; i--){
    for(j = 0; j < data->threadArgs->messages->messagesIndex[i].totalMentions; j++){
      if(!strcmp(data->threadArgs->username, data->threadArgs->messages->messagesIndex[i].mentions[j])){
        strcpy(sGetMessages->messages[totalMessages].sender, data->threadArgs->messages->messagesIndex[i].sender);
        strcpy(sGetMessages->messages[totalMessages].message, data->threadArgs->messages->messagesIndex[i].message);
        totalMessages++;
        break;
      }
    }
  }

  return;
}

#endif
