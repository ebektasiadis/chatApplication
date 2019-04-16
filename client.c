#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "defines.h"
#include "structs.h"
#include "clientFunctions.h"

int socketv;
//Ο πίνακας που κάνουμε map τις εντολές
router_t indexes[] = {
  {NEW_USER, sizeof(C_newUser), sizeof(S_newUser)},
  {GET_TOTAL_USERS, 0, sizeof(S_getTotalUsers)},
  {GET_USER, sizeof(C_getUser), sizeof(S_getUser)},
  {POST_MESSAGE, sizeof(C_postMessage), 0},
  {GET_MESSAGES, 0, sizeof(S_getMessages)},
};

//Συνάρτηση που κάνει handle το CTRL+C που πατάει ο client
void sigHandler(int sigID){ //Εάν πατηθεί CTRL+C μπες εδω
  sendCmd(DISCONNECT, NULL);  //Στείλε στον server την εντολή DISCONNECT
  exit(1);  //Τερμάτισε
}


int main(int argc, char* argv[]){
  int connectv;
  struct sockaddr_in socketAddr;
  C_newUser cNewUser;

  signal(SIGINT, sigHandler); // Πλέον το σήμα SIGINT γίνεται handle σύμφωνα με την συνάρτηση sigHandler
  signal(SIGTSTP, sigHandler);  // Πλέον το σήμα SIGTSTP γίνεται handle σύμφωνα με την συνάρτηση sigHandler

  socketv = socket(AF_INET, SOCK_STREAM, 0);  //Φτιάξε το socket

  if(socket<0){
      perror("socket");
      exit(1);
  }

  //Ρυθμίσεις για το socket
  socketAddr.sin_family = AF_INET;
  socketAddr.sin_port = htons(atoi(argv[1]));
  socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  //Κάνε connect στο server με βάση τις παραπάνω ρυθμίσεις
  connectv = connect(socketv, (struct sockaddr *)&socketAddr, sizeof(socketAddr));

  if(connectv<0){
      perror("connect");
      exit(1);
  }

  cNewUser = usernameInput(); //Δες ClientFunctions.h

  char msg[MAX_MESSAGE_LENGTH], msgToBeTokenized[MAX_MESSAGE_LENGTH]; //Δύο πίνακες ο ένας έχει το μήνυμα του χρήστη και το άλλο είναι αντιγραφή αυτόυ για να κάνουμε την strtok (γιατί η strtok καταστρέφει το string)
  char* word; //Αποθηκεύω κάθε φορά τη λέξη που βγαίνει απο την strtok
  char** words = (char**)malloc(sizeof(char*)); //Δεσμέυω χώρο για τον πίνακα words που βάζω μέσα τις λέξεις που γίνονται tokenized
  int totalWords; //Counter

  printf("Connected. Type /? or /help to view the available commands.\n");

  while(true){
    fgets(msg, sizeof(msg), stdin); //Όμοια λειτουργία με scanf, παίρνω είσοδο απο το χρήστη.
    msg[strlen(msg)-1] = '\0';  //Βάλε στο τέλος '\0' για να τερματίσει το string
    totalWords = 0; //Αρχικοποίηση counter
    system("clear");  //Εντολή για να καθαρίσει το output της οθόνης

    strcpy(msgToBeTokenized, msg);  //Αντιγραφή του μηνύματος απο τον πίνακα msg στον πίνακα msgToBeTokenized για το λόγο που περιγράψαμε παραπάνω.
    word = strtok(msgToBeTokenized, " "); //Ξεκίνα την strtok, με delimeter το κενό. Δηλαδη πάρε το string, και κάθε φορά που βρίσκεις κενό

    while(word != NULL) { //Για όσο βρίσκεις λέξεις
      totalWords++; //Αύξησε τον counter
      words = (char**)realloc(words, sizeof(char*)*totalWords); //Μεγάλωσε τον πίνακα κατα 1 γραμμή για να μπει η λέξη με realloc
      words[totalWords-1] = (char*)malloc(sizeof(char) * strlen(word)); //Μεγάλωσε τον πίνακα όσες στήλες είναι το μέγεθος της λέξης με realloc
      strcpy(words[totalWords-1], word);  //Βάλε τη λέξη στο πίνακα που μόλις μεγάλωσες.
      word = strtok(NULL, " "); //Ξανά strtok την επόμενη λέξη
    }

    if(totalWords == 0) //Εάν δεν βρεις λέξεις απλά κάνε continue
      continue;

    if(!strcmp(words[0], "/u") || !strcmp(words[0], "/users")){
      showUsers();  //Δες ClientFunctions.h
    }else if(!strcmp(words[0], "/r") || !strcmp(words[0], "/refresh")){
      showMessages(); //Δες ClientFunctions.h
    }else if(!strcmp(words[0], "/e") || !strcmp(words[0], "/exit")){
      sendCmd(DISCONNECT, NULL);  //Επικοινωνία με server
      exit(0);
    }else if(!strcmp(words[0], "/?")  || !strcmp(words[0], "/help")){
      printf("Chat Application\n");
      printf("/u OR /users   :: Show online users\n");
      printf("/r OR /refresh :: Refresh messages\n");
      printf("/e OR /exit    :: Exit application (Can be happen with CTRL+C too)\n");
    }else if(strlen(words[0]) > 0 && words[0][0] == '@' && totalWords > 1){ //Εάν θέλει να στείλει μύνημα 'έχει μπροστά @'τότε
      sendCmd(POST_MESSAGE, msg); //Στείλε το μύνημα στο server με κωδικό POST_MESSAGE
      showMessages(); //Δες ClientFunctions.h
    }else{
      printf("Commmand not found\n"); //Αν γράψει οτιδήποτε άλλο, απλά πες του ότι δεν υπάρχει αυτό το commmand.
    }
  }

  return 0;
}

void* sendCmd(int cmd, void* data){ //Συνάρτηση που μιλάω με το server
  void* response;

  write(socketv, &cmd, sizeof(cmd));  //Γράψε στο server το είδος της εντολής που να περιμένει (ώστε αν ξερει πόσα bytes να κανει read)
  if(cmd >= 0){ //Εάν είναι να του στείλεις κάποια εντολή με cmd_code >= 0
    response = (void*)malloc(indexes[cmd].lServer); //Δέσμευσε χώρο στη μεταβλητή response, τόσο όσο ορίζεται απο το είδος της εντολής cmd και περιμένουμε απο το server
    if(indexes[cmd].lClient > 0){ //Εάν είσαι σε φάση που θες να στείλεις κάτι, και για αυτό το μέγεθος του client > 0
      write(socketv, data, indexes[cmd].lClient); //Γράψε στο server αυτό που θες να του στείλεις
    }
    if(indexes[cmd].lServer > 0){ //Εάν είσαι σε φάση που θες να λάβεις κάτι, και για αυτό το μέγεθος του server > 0
      read(socketv, response, indexes[cmd].lServer);  //Διάβασε απο το server αυτο που θέλει να σου στείλει
    }
  }

  return response;
}
