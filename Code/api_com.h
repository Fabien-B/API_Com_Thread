#ifndef API_COM_H_INCLUDED
#define API_COM_H_INCLUDED
#include <pthread.h>

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
    struct timeval *tv;     //to use with gettimeofday
} message;

int initMsg();
int aboMsg(communication *);
int sendMsg(communication * mycom, int id_dest, void * contenu);
int recvMsg(communication *, message **msg);
int getNbMsg(communication *);
int desaboMsg(communication *);
int finMsg();

typedef enum com_errors {
    SUCCESS,
    ALREADY_LAUNCH,
    INIT_ERROR,
    NO_SERVICE,
    ID_IN_USE,
    MAX_ABO,
    NO_ABO,
    ID_UNKNOWN,
    NO_MSG,
    SERVICE_USED,
    TECH_ERROR
} com_errors;

//l'abonnement est une opération spéciale, elle ne passe pas par le meme moyen.
typedef enum operations {
    NO_OP,
    SENDMSG,
    RECVMSG,
    GETNBMSG,
    DESABO,
    ISABO,
    GETNBABO,
    CLOSESERVICE
} operations;

#endif // API_COM_H_INCLUDED
