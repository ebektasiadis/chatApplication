#ifndef _CLIENT_FUNCTIONS_H_
#define _CLIENT_FUNCTIONS_H_

void* sendCmd(int cmd, void* data);

C_newUser usernameInput(){  //Συνάρτηση η οποία παίρνει το πρώτο input απο το χρήστη για το username του.
  void* output;
  //Δημιουργώ δυο μεταβλητές τύπου C_newUser και S_newUser. Δες structs.h 
  C_newUser cNewUser;
  S_newUser sNewUser;

  do{
    printf("Enter your username: ");
    fgets(cNewUser.username, sizeof(cNewUser.username), stdin); //Όμοια δουλειά με την scanf, αποθήκευση απάντησης χρήστη στο CNewUser.username
    cNewUser.username[strlen(cNewUser.username)-1] = '\0';  //Βάλε στο τέλος το '\0' για να τερματίσει το string
    output = sendCmd(NEW_USER, &cNewUser);  //Στείλε στον server ότι συνδέθηκε νέος χρήστης με επυτιχία. 
    sNewUser = *(S_newUser*)output; //Αυτό που θα σου γυρίσει η επικοινωνία με το server.
    switch(sNewUser.confirm){ //Ανάλογα το τι απάντηση θα πάρεις απο τον server ο οποίος επεξεργάστικε το username που του στείλαμε (αν είναι valid)
      case 0: break;  //Όλα οκ
      case 1: printf("Username length is not valid.\n"); break;
      case 2: printf("This username already exists.\n"); break;
      case 3: printf("You can not leave this field blank.\n"); break;
      case 4: printf("You can not use special characters.\n"); break;
      default: printf("Confirmation code %d has no message.\n", sNewUser.confirm); break;
    }
  }while(sNewUser.confirm != 0);  //Εαν γίνει επιτυχής σύνδεση του client
  return cNewUser;  //Επέστρεψε το struct
}

void showUsers(){ 
  system("clear");  //Καθάρισε την οθόνη
  void* output = sendCmd(GET_TOTAL_USERS, NULL);  //Μίλα στον server με κωδικό GET_TOTAL_USERS, ζήτα του δηλαδή το πίνακα με όλους τους διαθλεσιμους χρήστες
  S_getTotalUsers sGetTotalUsers = *(S_getTotalUsers*)output; //αποθήκευσε αυτο που σου επέστρεψε ο server, εδω μας επιστρέφει τον αριθμό των online clients
  //Φτιάχνω δύο μεταβλητές για τα struct που περιγράφουμε στο structs.h
  C_getUser cGetUser; 
  S_getUser sGetUser;
  int i;

  printf("Online Users(%d):\n", sGetTotalUsers.totalUsers); //Τύπωσε του online clients
  for(i = 0; i < sGetTotalUsers.totalUsers; i++){ //Κάνε request απο τον server όλα τα ονόματα των online clients. Πόσες φορές; Τόσες όσοι είναι οι συνολικοί online clients
    cGetUser.pos = i;
    output = sendCmd(GET_USER, &cGetUser);  //μίλα με server, και ζήτα του να εκτελέσει τη συνάρτηση GET_USER. Αποθήλευσε το return στο output
    sGetUser = *(S_getUser*)output; //Αποθήκευσε αυτο που σου επέστρεψ εο server (το κάθε username δηλαδή) στη μεταβλητή τύπου struct
    printf("%s ", sGetUser.username); //Print
  }
  printf("\n"); //Αλλαγή γραμμής.
}

void showMessages(){  //Συνάρτηση για να μυ εκτυπώσει όλα τα μηνύματα τα οποία προορίζονται για το συγκεκριμένο client
  int i;
  S_getMessages sGetMessages; //Δες structs.h
  void* output;
  char* word;

  printf("Showing %d most recent messages:\n", MAX_RETURN_MESSAGES);

  output = sendCmd(GET_MESSAGES, NULL); //Στείλε στον server εντολή ότι θες να σου στείλει τα μυνήματα για το χρήστη
  sGetMessages = *(S_getMessages*)output; //Αποθήκευσε αυτο που σου επσιτρέφει στο output. 
  for(i = MAX_RETURN_MESSAGES-1; i >= 0; i--){  //Ξεκίνα ανάποδα σοτν πίνακα με τα messages του client (ωστε να έχουμε τα recent πάνω πανω)
    if(strlen(sGetMessages.messages[i].message) > 0){ //Εάν υπάρχει μήνυμα στο ιστορικό
      printf("\033[1m%s: \033[0m", sGetMessages.messages[i].sender);  //Εκτύπωσε τον αποστολέα (με χρώμα)
      word = strtok(sGetMessages.messages[i].message, " "); //Κάνε strtok το μύνημα για να ξεχωρίσουμε αποστολές απο μήνυμα, για να το εκτυπωσουμε πιο ωραία

      while(word != NULL) {
        if(strlen(word) > 0 && word[0] == '@'){ //Εάν βρεις απσοοτλές (επειδή έχιε μπροστά το '@')
          printf("\033[33m%s\033[0m ", word); //Εκτύπωσε τον αποστολέα με χρωμα
        }else{
          printf("%s ", word);  //αλλιω απλά εκτύπωσε το υπόλοιπο μήνυμα κανονικά.
        }
        word = strtok(NULL, " "); //συνέχισε την strtok
      }
      printf("\n"); //αλλαγή γραμμής
    }
  }
}

#endif
