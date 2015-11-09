#ifndef API_COM_H_INCLUDED
#define API_COM_H_INCLUDED
#include <pthread.h>

typedef communication {
    int client_id;
    int operation;
    void * contenu;
    pthread_mutex_t * mutex;

} communication;

int initMsg();
int aboMsg(communication *);
int sendMsg(int my_id, int dest);
int recvMsg(int my_id);
int getNbMsg(int my_id);
int desaboMsg(int my_id);
int finMsg();


#endif // API_COM_H_INCLUDED
