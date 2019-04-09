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

#endif
