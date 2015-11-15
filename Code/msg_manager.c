#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "types_private.h"
#include "api_com.h"
#include <string.h>

int sendMsg(communication * mycom, int id_dest, void * contenu, int data_size)
{
    if(_thread_gest==NULL)
    {
        return NO_SERVICE;
    }
    mycom->operation = SENDMSG;
    mycom->dest_id = id_dest;

    message * mess_to_send = NULL;          //création d'un message
    mess_to_send = malloc(sizeof(message));
    if(mess_to_send == NULL){
        return TECH_ERROR;
    }

    mess_to_send->expid = mycom->client_id; //signature expéditeur

    mess_to_send->contenu = NULL;           //création du contenu du message
    mess_to_send->contenu = malloc(data_size);
    if(mess_to_send->contenu == NULL){
        return TECH_ERROR;
    }

    memcpy(mess_to_send->contenu,contenu,data_size);    //copie des données (l'émétteur peut alors supprimer l'original sans affecter la réception)

    mycom->contenu = mess_to_send;
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
