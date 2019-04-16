#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "defines.h"
#include "structs.h"
#include "serverFunctions.h"

router_t indexes[] = {  //Δημιουργώ το struct που ουσιαστικά είναι ο πίνακας με το mapping των commands
  {NEW_USER, sizeof(C_newUser), sizeof(S_newUser), newUser},  //Δημιουργία νέου User
  {GET_TOTAL_USERS, 0, sizeof(S_getTotalUsers), getTotalUsers}, //Επιστροφή όλων των online users
  {GET_USER, sizeof(C_getUser), sizeof(S_getUser), getUser},  // Επιστροφή πληροφοριών για τον χρήστη με το index που βρίσκεται στο Client struct.
  {POST_MESSAGE, sizeof(C_postMessage), 0, postMessage},  //Ετοιμασία του μυνήματος για αποστολή
  {GET_MESSAGES, 0, sizeof(S_getMessages), getMessages},  //Ετοιμασία για παραλαβή μυνήματος.
};

void disconnect(threadArgs_t threadArgs){ //Ουσιαστικά αφαιρεί τον χρήστη που έφυγε απο τη λίστα με τους online χρήστες.
  int i, target;
  char tmp[MAX_USERNAME_LENGTH], tmp2[MAX_USERNAME_LENGTH];

  if(strlen(threadArgs.username) == 0)  //Αν έφυγε πριν βάλει καν username μην κάνεις τίποτα αφού δεν πρόλαβε να μπει στη λίστα με τους online ετσι και αλλιως.
    return;

  for(i = 0; i < *threadArgs.totalConnected; i++){
    if(!strcmp(threadArgs.username, (*threadArgs.usernames)[i])){
      target = i; //Τρέξε το πίνακα με τους online χρηστες και βρες αυτον που έφυγε. Αποθήκευσε τη θέση του.
      break;
    }
  }

  //Κλασσικός τρόπος swap μεταξύ του χρήστη που έφυγε με τον τελευταίο στο πίνακα.
  if(*threadArgs.totalConnected-1 > 0){
    strcpy(tmp, (*threadArgs.usernames)[*threadArgs.totalConnected-1]);
    strcpy(tmp2, (*threadArgs.usernames)[target]);
    strcpy((*threadArgs.usernames)[*threadArgs.totalConnected-1], tmp2);
    strcpy((*threadArgs.usernames)[target], tmp);
  }
  //Αφου παω τον χρήστη που έφυγε στο τέλος του πίνακα κάτω, κάνω realloc τον πίνακα και τον μικραίνω κατα ένα. Επομένως ο τελευταιος χάνεται απο τον πίνακα με τους online.
  *threadArgs.usernames = (char**)realloc(*threadArgs.usernames, sizeof(char*) * --(*threadArgs.totalConnected));
}

void* core(void* args){ //Συνάρτηση που διαχειρίζεται το thread του client. Παίρνει σαν είσοδο το struct με τα στοιχεία του χρήστη που δημιουργήσαμε
  threadArgs_t threadArgs = *(threadArgs_t*)args; //type-casting του void* struct που πέρασα ως είσοδο, προκειμένου αν μπορώ αν το επεξεργαστώ μέσα στη συνάρτηση

  while(true){
    int cmd;
    void* input;
    data_t data;  //Δημιουργώ το struct για τα δεδομένα που θα διαχειριστεί και θα στείλει ο server

    read(threadArgs.sID, &cmd, sizeof(int));  //Περιμένω να διαβάσω απο τον χρήστη (στην αρχή περιμένω το username)
    printf("Received cmd: %d\n", cmd);  //DEBUG print
    if(cmd >= 0){ //Εάν πάρω απο το χρήστη οποιαδήποτε εντολή με cmd code > 0 (δες πίνακα με defines.h) κάνε τα παρακάτω
      printf("For this command you will receive %zu bytes and send back %zu bytes.\n", indexes[cmd].lClient, indexes[cmd].lServer); //DEBUG print πόσα mb να περιμένω και πόσα πρέπει να στείλω
      input = (void*)malloc(indexes[cmd].lClient);  //Κάνω type cast απο void* και δεσμεύσω χώρο, τόσο όσο είναι τα bytes που περιμένω απο το χρήστη στη μεταβλητή input
      if(indexes[cmd].lClient > 0){ //Εάν περιμένω να μου στείλει κάτι ο χρήστης, τότε μπες και διάβασε αυτά που θέλει να σου στείλει
        printf("Read from client %lu bytes\n", read(threadArgs.sID, input, indexes[cmd].lClient));  //Διαβάζω τι έχει να μου στείλει ο χρήστης.
        data.dClient = input; //Αποθήκευση των "bytes" που μου έστειλε ο χρήστης στο data.dClient
      }

      data.threadArgs = &threadArgs;  //Αποθηκεύω τα στοιχεία του χρήστη στο data.threadArgs
      data.dServer = (void*)malloc(indexes[cmd].lServer); //Δεσμέυω χώρο για αυτά που θα στείλω τόσο όσο ξέρω ότι θα χρειαστεί να στείλω.
      indexes[cmd].function(&data); //Του λέω πήγενε στην συνάρτηση που σου ορίζει ο πίνακας των commands (βάση της εντολής), και του περνάω σαν είσοδο το "data" που περιέχει όλα τα στοιχεία που πρέπει να πάνε για επεξεργασία. Σαν να τα έχω πακετάρει δηλαδή και να στελνω όλα μαζί μέσα στο κουτί "data"

      if(indexes[cmd].lServer > 0){ //Εαν έχει να στείλει κάτι ο server μπες μεσα στην if και κάνε write όπου είναι να κάνεις write τα επεξεργασμένα πλέον δεδομένα.
        printf("Wrote on client %lu bytes\n", write(threadArgs.sID, data.dServer, indexes[cmd].lServer)); //Στέλνω τα δεδομένα απο το server σε client.
      }

    }else{
      switch(cmd){
        case DISCONNECT: disconnect(threadArgs); pthread_exit(NULL); break; //Σε περίπτωση που ο χρήστης στείλει μήνυμα εξόδου, κάνε τον disconect (αφαίρεσε τον απο τη λίστα των online) και κλείσε το thread του.
      }
    }
  }
}

int main(int argc, char* argv[]){
  int totalConnected = 0;

  int bindv, socketv, clientv, threadv; //Μεταβλητές για τα sockets και τα thread
  struct sockaddr_in socketAddr;  //Struct για τα sockets

  threadArgs_t threadArgs;  //Δημιουργώ μια μεταβλητη τύπου threadArgs_t την οποία θα γεμίσω στη συνέχεια με τα στοιχεία του νέου χρήστη

  socketv = socket(AF_INET, SOCK_STREAM, 0);  //Δημιουργία Socket

  if(socket<0){
    perror("socket");
    exit(1);
  }

  //Ρυθμίσεις για το socket
  socketAddr.sin_family = AF_INET;
  socketAddr.sin_port = htons(atoi(argv[1]));
  socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  bindv = bind(socketv, (struct sockaddr *)&socketAddr, sizeof(socketAddr));  //Ένωη όλων των παραπάνω ρυθμίσεων του socket

  if(bindv<0){
    perror("bind");
    exit(1);
  }

  if(listen(socketv, 1) < 0){ //Περιμένω για εισερχόμενες συνδέσεις.
    perror("listen");
    exit(1);
  }

  char** usernames = (char**)malloc(sizeof(char*)); //Δημιουργία και δέσμευση μνήμης για το σισδιάστατο πίνακα usernames
  messages_t messages;  //Δημιουργία struct τύπου messages_t
  messages.totalMessages = 0; //Αρχικοποίηση μυνημάτων σε 0
  messages.messagesIndex = (messageIndex_t*)malloc(sizeof(messageIndex_t)); //Δέσμευση χώρου για το messagesIndex. Pending...

  //Threading.
  while(true){
    clientv = accept(socketv, NULL , NULL); //Δέχομαι ένα νέο client
    if(clientv < 1){
      perror("accept");
    }

    printf("New user logged in and found username pointer at %p\n", &usernames);  //DEBUG print

    if(pthread_create(&threadArgs.tID, NULL, core, &threadArgs) != 0){  //Δημιουργία του thread για το νέο client με την εντολή "pthread_create"
      perror("pthread_create");                                         //Λέω στο πρόγραμμα μου ότι το thread αυτό θα το διαχειρίζεται η συνάρτηση "core"
    }else{
      //Γεμίζω το struct threadArgs_t με τα στοιχεία του νέου χρήστη που μόλις συνδέθηκε
      threadArgs.sID = clientv;
      threadArgs.usernames = &usernames;
      threadArgs.messages = &messages;
      threadArgs.totalConnected = &totalConnected;
    }

    printf("User has been connected\n");
  }

  return 0;
}
