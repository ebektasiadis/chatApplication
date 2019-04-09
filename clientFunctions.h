#ifndef _CLIENT_FUNCTIONS_H_
#define _CLIENT_FUNCTIONS_H_

void* sendCmd(int cmd, void* data);

C_newUser usernameInput(){
  void* output;
  C_newUser cNewUser;
  S_newUser sNewUser;

  do{
    printf("Enter your username: ");
    fgets(cNewUser.username, sizeof(cNewUser.username), stdin);
    cNewUser.username[strlen(cNewUser.username)-1] = '\0';
    output = sendCmd(NEW_USER, &cNewUser);
    sNewUser = *(S_newUser*)output;
    switch(sNewUser.confirm){
      case 0: break;
      case 1: printf("Username length is not valid.\n"); break;
      case 2: printf("This username already exists.\n"); break;
      case 3: printf("You can not leave this field blank.\n"); break;
      case 4: printf("You can not use special characters.\n"); break;
      default: printf("Confirmation code %d has no message.\n", sNewUser.confirm); break;
    }
  }while(sNewUser.confirm != 0);
  return cNewUser;
}

void showUsers(){
  system("clear");
  void* output = sendCmd(GET_TOTAL_USERS, NULL);
  S_getTotalUsers sGetTotalUsers = *(S_getTotalUsers*)output;
  C_getUser cGetUser;
  S_getUser sGetUser;
  int i;

  printf("Online Users(%d):\n", sGetTotalUsers.totalUsers);
  for(i = 0; i < sGetTotalUsers.totalUsers; i++){
    cGetUser.pos = i;
    output = sendCmd(GET_USER, &cGetUser);
    sGetUser = *(S_getUser*)output;
    printf("%s ", sGetUser.username);
  }
  printf("\n");
}

void showMessages(){
  int i;
  S_getMessages sGetMessages;
  void* output;
  char* word;

  printf("Showing %d most recent messages:\n", MAX_RETURN_MESSAGES);

  output = sendCmd(GET_MESSAGES, NULL);
  sGetMessages = *(S_getMessages*)output;
  for(i = MAX_RETURN_MESSAGES-1; i >= 0; i--){
    if(strlen(sGetMessages.messages[i].message) > 0){
      printf("\033[1m%s: \033[0m", sGetMessages.messages[i].sender);
      word = strtok(sGetMessages.messages[i].message, " ");

      while(word != NULL) {
        if(strlen(word) > 0 && word[0] == '@'){
          printf("\033[33m%s\033[0m ", word);
        }else{
          printf("%s ", word);
        }
        word = strtok(NULL, " ");
      }
      printf("\n");
    }
  }
}

#endif
