#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "types_private.h"
#include "api_com.h"


int sendMsg(communication * mycom, int id_dest, void * contenu)
{
    if(_thread_gest==NULL)
    {
        return NO_SERVICE;
    }
    mycom->operation = SENDMSG;
    mycom->dest_id = id_dest;
    mycom->contenu = contenu;
    mycom->retour = -1;

    pthread_mutex_lock(mycom->mutex);  //je lock mon mutex pour que le signal ne me soit envoyé que pendant mon wait
    pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
    while(mycom->retour == -1)              //tant que le gestionnaire n'a pas fait son action...
    {
        pthread_cond_wait(mycom->signal_gestionnaire, mycom->mutex);    //on attend
    }

    int code_retour = mycom->retour;
    mycom->operation = NO_OP;
    pthread_mutex_unlock(mycom->mutex);
    printf("fin send\n");
    return code_retour;
}


int recvMsg(communication * mycom, message **msg)   //**msg : pointeur sur pointeur sur message -> on modifie le pointeur sur message.
{
    if(_thread_gest==NULL)
    {
        return NO_SERVICE;
    }

    mycom->operation = RECVMSG;
    mycom->retour = -1;

    pthread_mutex_lock(&_mutex_abo);
    pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
    pthread_mutex_unlock(&_mutex_abo);

    while(mycom->retour == -1)
    {
        pthread_cond_wait(mycom->signal_gestionnaire, mycom->mutex);
    }

    if(mycom->retour == SUCCESS)
    {
        *msg = mycom->contenu;  //pointeur sur message vaut le pointeur retourné par le gestionnaire
    }

    mycom->operation = NO_OP;
    return mycom->retour;

}
