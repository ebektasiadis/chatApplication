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

  //TODO: NOT ALLOW SPECIAL CHARACTERS.
  (*data->threadArgs->totalConnected)++;
  *data->threadArgs->usernames = (char**)realloc(*data->threadArgs->usernames, sizeof(char*) * (*data->threadArgs->totalConnected));
  (*data->threadArgs->usernames)[*data->threadArgs->totalConnected-1] = (char*)malloc(sizeof(char) * (MAX_USERNAME_LENGTH + 1));
  strcpy((*data->threadArgs->usernames)[(*data->threadArgs->totalConnected)-1], cNewUser.username);
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

#endif
