#ifndef API_COM_H_INCLUDED
#define API_COM_H_INCLUDED
#include <pthread.h>

typedef struct communication {
    int client_id;
    int operation;
    void * contenu;
    pthread_mutex_t mutex;
    pthread_cond_t signal_gestionnaire;

} communication;

int initMsg();
int aboMsg(communication *);
int sendMsg(int my_id, int dest);
int recvMsg(int my_id);
int getNbMsg(int my_id);
int desaboMsg(int my_id);
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
    SERVICE_USED,
    TECH_ERROR
} com_errors;

typedef enum operations {
    ABO,
    SENDMSG,
    RECVMSG,
    GETNBMSG,
    DESABO,
    ISABO,
    GETNBABO,
    CLOSESERVICE
} operations;

#endif // API_COM_H_INCLUDED
