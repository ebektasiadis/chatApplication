#ifndef _SERVER_FUNCTIONS_H_
#define _SERVER_FUNCTIONS_H_

void newUser(data_t* data){
  C_newUser cNewUser = *(C_newUser*)data->dClient;  //Δημιουργία της μεταβλητης τύπου struct και αποθήκευση σε αυτης τα δεδομένα απο τον client
  S_newUser* sNewUser;  //Pointer τύπου S_newUser
  sNewUser = data->dServer; //Γέμισμα με τα δεδομένα του server

  if(strlen(cNewUser.username) > MAX_USERNAME_LENGTH){  //Εάν το μέγεθος του username είναι μεγαλύτερο απο το επιτρεπτό, γύρνα confrim code = 1
    sNewUser->confirm = 1;
    return;
  }

  int i;
  //Τρέξε όλο το πίνακα με τα usernames και αν βρεις ίδια, κάνε το confirm code = 2
  for(i = 0; i < *data->threadArgs->totalConnected; i++){
    if(strcmp(cNewUser.username, (*data->threadArgs->usernames)[i]) == 0){
      sNewUser->confirm = 2;
      return;
    }
  }

  //Εάν δεν γράψει καθόλου username, δώσε confirm code = 3
  if(strlen(cNewUser.username) == 0){
    sNewUser->confirm = 3;
    return;
  }

  //Αν βρεις special characters δώσε confirm code = 4
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

  (*data->threadArgs->totalConnected)++;  //Αν όλα καλά, αύξησε τους total connected
  //Μεγάλωσε τον πίνακα με τα usernames κατα μια γραμμή
  *data->threadArgs->usernames = (char**)realloc(*data->threadArgs->usernames, sizeof(char*) * (*data->threadArgs->totalConnected));
  ////Μεγάλωσε τη γραμμή που έκανες όσες στήλες χρειάζεται
  (*data->threadArgs->usernames)[*data->threadArgs->totalConnected-1] = (char*)malloc(sizeof(char) * (MAX_USERNAME_LENGTH));
  //Αντέγραψε στον πίνακα αυτό το username 
  strcpy((*data->threadArgs->usernames)[(*data->threadArgs->totalConnected)-1], cNewUser.username);
  strcpy(data->threadArgs->username, cNewUser.username);
  sNewUser->confirm = 0;  //Αφού ολοκληρώθηκαν τα παράνω στείλε confirm code = 0
  return;
}

void getTotalUsers(data_t* data){ 
  S_getTotalUsers* sGetTotalUsers;  //Pointer μεταβλητή για το struct S_getTotalUsers
  sGetTotalUsers = data->dServer; //Γεμίσω τη μεταβλητη μου με τα δεδομένα απο το server.

  sGetTotalUsers->totalUsers = *data->threadArgs->totalConnected; //Αποθήκευσε τον αριθμό των συνολικών online clients στην μεταβλητής μας.
  return;
}

void getUser(data_t* data){
  C_getUser cGetUser = *(C_getUser*)data->dClient;   //Γεμίσω τη μεταβλητη μου με τα δεδομένα απο το client.
  S_getUser* sGetUser;    
  sGetUser = data->dServer;  //Γεμίσω τη μεταβλητη μου με τα δεδομένα απο το server.

  strcpy(sGetUser->username, (*data->threadArgs->usernames)[cGetUser.pos]); //Κάνω αντιγραφή το username απο το struct στη μεταβλητλη μου.
  return;
}

//NOTES
void postMessage(data_t* data){
  int i, curMessage;
  char* word; //Το θέλουμε για να βάζουμε τη λέξη απο την strtok κάθε φορά
  char messageToBeTokenized[MAX_MESSAGE_LENGTH];
  C_postMessage cPostMessage = *(C_postMessage*)data->dClient;  //Δημιουργία μεταβλητής τύπου struct (C_postMessage) , και κατευθείαν ανάθεση σε αυτήν τα δεδομένα απο τον client, φού πρώτα τα κάνουμε type-cast απο void* τον τύπο του struct

  data->threadArgs->messages->totalMessages++;  //Αύξησε τα συνολικά μηνύματα
  curMessage = data->threadArgs->messages->totalMessages-1; //Βάζω στην μεταβλήτη την θέση του τελευταίου μηνύματος
  data->threadArgs->messages->messagesIndex = (messageIndex_t*)realloc(data->threadArgs->messages->messagesIndex, sizeof(messageIndex_t) * (data->threadArgs->messages->totalMessages));  //Αλυξησε τον πίνακα με τα totalMessages κατα μια θέση
  strcpy(data->threadArgs->messages->messagesIndex[curMessage].sender, data->threadArgs->username); //Θέσε στην μεταβλητή 'sender' του struct το username του αποστολέα
  strcpy(data->threadArgs->messages->messagesIndex[curMessage].message, cPostMessage.message);  //Θέσε στη μεταβλητή 'message' του struct το μήνυμα του αποστολέα
  printf("%s sent: %s\n", data->threadArgs->messages->messagesIndex[curMessage].sender, data->threadArgs->messages->messagesIndex[curMessage].message); //DEBUG PRINT για τον server, το μήνυμα και τον αποστολέα
  data->threadArgs->messages->messagesIndex[curMessage].totalMentions = 0;  //Θέσε την μεταβλητή 'totalMentions' = 0

  strcpy(messageToBeTokenized, data->threadArgs->messages->messagesIndex[curMessage].message);  //ΌΠως είπαμε και σε άλλο σημείο που κάνουμε strtok, η strtok καταστρέφει το αρχικό string για αυτο το αποθηκεύω πρώτα σε ένα άλλο και το οποίο κάνω tokenized
  word = strtok(messageToBeTokenized, " "); //Κάνω strtok με delimeter το κενό ' '.

  while(word != NULL) { //Για όοσ βρίσκεις λέξεις
    if(strlen(word) > 0 && strlen(word) <= MAX_USERNAME_LENGTH+1 && word[0] == '@'){  //Εάν έχει μπροστά '@' (δηλαδή είναι το μήνυμα να πάει εκεί), και είναι valid το username που δόθηκε τότε:
      data->threadArgs->messages->messagesIndex[curMessage].totalMentions++;  //Αύξησε τα mentions 
      data->threadArgs->messages->messagesIndex[curMessage].mentions = (char**)realloc(data->threadArgs->messages->messagesIndex[curMessage].mentions, sizeof(char*) * data->threadArgs->messages->messagesIndex[curMessage].totalMentions);  //Αύξησε το πίνακα με mentions κατα ένα
      data->threadArgs->messages->messagesIndex[curMessage].mentions[data->threadArgs->messages->messagesIndex[curMessage].totalMentions-1] = (char*)malloc(MAX_USERNAME_LENGTH * sizeof(char));  //Δέσμευσε όσες στήλες χρειάζονται σε αυτή την νέα γραμμή, για να μπει το username που έγινε mention.
      for(i = 1; i < strlen(word); i++){
        data->threadArgs->messages->messagesIndex[curMessage].mentions[data->threadArgs->messages->messagesIndex[curMessage].totalMentions-1][i-1] = word[i]; //Βάλε την λέξη (username που έγινε mention) στον πίνακα για τον οποίο μόλις δεσμεύσαμε χώρο. 
      }
      data->threadArgs->messages->messagesIndex[curMessage].mentions[data->threadArgs->messages->messagesIndex[curMessage].totalMentions-1][i-1] = '\0';  //Βάλε στο τέλος του string '\0' escape character για να τερματίσει σωστά το string
    }
    word = strtok(NULL, " "); //Συνέχισε την strtok
  }
  return;
}

void getMessages(data_t* data){
  S_getMessages* sGetMessages;  //Pointer στο struct S_getMessage
  sGetMessages = data->dServer; //Περνάω στη μεταβλήτη του struct τα bytes του server
  int totalMessages = 0;  //Counter

  int i,j;  
  //Ψάχνω τον πίνακα με τα μηνύματα απο το τέλος στην αρχή (για να βγουν τα recent πανω πανω στο τέλος κατευθείαν)
  for(i = data->threadArgs->messages->totalMessages-1; i >= 0 ; i--){
    for(j = 0; j < data->threadArgs->messages->messagesIndex[i].totalMentions; j++){
      if(!strcmp(data->threadArgs->username, data->threadArgs->messages->messagesIndex[i].mentions[j])){  //Εάν βρεις μήνυμα που σε έχουν κάνει mention τότε:
        strcpy(sGetMessages->messages[totalMessages].sender, data->threadArgs->messages->messagesIndex[i].sender);  //Κάνε copy το username του sender μεσα στο struct μας
        strcpy(sGetMessages->messages[totalMessages].message, data->threadArgs->messages->messagesIndex[i].message);  //Κάνε copy το μήνυμα μέσα στο struct μας
        totalMessages++;  //Αύξησε τον counter με τα συνολικά μηνύματα
        break;
      }
    }
  }

  return;
}

#endif
