#ifndef API_COM_H_INCLUDED
#define API_COM_H_INCLUDED
#include <pthread.h>
#include <sys/time.h>

typedef struct communication {
    int client_id;
    int operation;
    int retour;
    int dest_id;
    void * contenu;
    pthread_mutex_t * mutex;
    pthread_cond_t * signal_gestionnaire;

} communication;

typedef struct message {
    void * contenu;
    int expid;
    struct timezone tz;
    struct timeval tv;
} message;

int initMsg();
int aboMsg(communication *, int id);
int sendMsg(communication * mycom, int id_dest, void * contenu, int data_size);
int recvMsg(communication *, message **msg);
int getNbMsg(communication *);
int desaboMsg(communication *);
int finMsg();

typedef enum com_errors {
    SUCCESS,                //0
    ALREADY_LAUNCH,         //1
    INIT_ERROR,             //2
    NO_SERVICE,             //3
    ID_IN_USE,              //4
    MAX_ABO,                //5
    NO_ABO,                 //6
    ID_UNKNOWN,             //7
    NO_MSG,                 //8
    SERVICE_USED,           //9
    TECH_ERROR,             //10
    CLOSESERVICE_FORCED     //11
} com_errors;

//l'abonnement est une opération spéciale, elle ne passe pas par le meme moyen.
typedef enum operations {
    NO_OP,              //0
    SENDMSG,            //1
    RECVMSG,            //2
    GETNBMSG,           //3
    DESABO,             //4
    ISABO,              //5
    GETNBABO,           //6
    CLOSESERVICE,       //7
    ABO
} operations;

#endif // API_COM_H_INCLUDED
