#ifndef _STRUCTS_H_
#define _STRUCTS_H_


  //pthread on create struct variable

  //server struct
  typedef struct {
    int     totalMentions;  //μεταβλητή που μας δείχνει πόσοι έχουν γίνει συνολικά mention για το μύνημα.
    char**  mentions; //δισδιάστατος πίνακας που περιέχει τα mentions με τα usernames που θα σταλθέι το μύνημα.
    char    sender[MAX_USERNAME_LENGTH];  //πίνακας που περιέχει το username του αποστολέα.
    char    message[MAX_MESSAGE_LENGTH];  //πίνακας που περιέχει το μήνυμα.
  } messageIndex_t;

  typedef struct {
    int             totalMessages;  //counter για τα συνολικά μυνήματα
    messageIndex_t* messagesIndex;  //pointer για το struct, που ουσιαστικά περιέχει τις λεπτομέρειες για τα μηνύματα αυτά
  } messages_t;

  typedef struct {
    pthread_t   tID;  //thread id
    int         sID;  //socket id 
    int*        totalConnected; //Counter πόσοι συνολικά είναι συνδεδεμένοι εκείνη τη στιγμή.
    char        username[MAX_USERNAME_LENGTH];  //Πίνακας που κρατάει το username του χρήστη.
    char***     usernames;  //Δισδιάστατος πίνακας με όλα τα usrnames.
    messages_t* messages; //pointer στο struct messages_t
  } threadArgs_t; //Περιέχει τα στοιχελια για τον κάθε client την στιγμλη που δημιουργείτε.

  typedef struct {
    void*         dClient;  //Τα δεδομένα του Client τύπου void* 
    void*         dServer;  //Τα δεδομένα του Server τύπου void* 
    threadArgs_t* threadArgs; //Pointer στο struct
  } data_t; //Η είσοδος για όλες μας τις συναρτήσεις.

  typedef struct {
    int     cmd;  //Εντολή για το τι είδους μήνυμα στέλνω, βάση του πίνακα με τις ορισμένες εντολές
    size_t  lClient;  //Μέγεθος για τον client. Για να ξέρει ο Client τι μέγεθος να κάνει write / read
    size_t  lServer;  //Μέγεθος για τον server. Για να ξέρει ο server τι μέγεθος να κάνει write / read
    void    (*function)(data_t*); //Pointer υναρτησης. το void δηλώνει το τι επιστρέφει η συνάρτηση, και το data_t* η είσοδος της συνάρτησης 
  } router_t; 

  //client structs
  typedef struct {
    char username[MAX_USERNAME_LENGTH]; //Πίνακας στον οποίο αποθηκεύω το username του client 
  } C_newUser;

  typedef struct {
    int pos;
  } C_getUser;

  typedef struct {
    char message[MAX_MESSAGE_LENGTH]; //Πίνακας στον οποίο αποθηκεύω το μύνημα που είναι να σταλθεί.
  } C_postMessage;

  //server structs
  typedef struct {
    int confirm;  //Μεταβλητή η οποία μας κάνει confirm ότι συνδέθηκε ένας νέος χρήστης.
  } S_newUser;

  typedef struct {
    int totalUsers; //Μεταβλητή η οποία περιέχει τον αριθμό όλων τω συνδεδεμένων χρηστών.
  } S_getTotalUsers;

  typedef struct {
    char username[MAX_USERNAME_LENGTH]; //Πίνακας στον οποίο αποθηκεύω το username ενός client
  } S_getUser;

  typedef struct {
    messageIndex_t messages[MAX_RETURN_MESSAGES]; //Struct τύπου messageIndex_t στο οποίο αποθηκεύω όλα τα μυνήματα.
  } S_getMessages;

#endif
