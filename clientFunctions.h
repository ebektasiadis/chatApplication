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
    printf("Confirmation: %d\n", sNewUser.confirm);
    switch(sNewUser.confirm){
      case 0: break;
      case 1: printf("Username length is not valid.\n"); break;
      case 2: printf("This username already exists.\n"); break;
      default: printf("Confirmation code %d has no message.\n", sNewUser.confirm); break;
    }
  }while(sNewUser.confirm != 0);
  return cNewUser;
}

#endif
