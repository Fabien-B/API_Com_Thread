#include <stdio.h>
#include "types_private.h"
#include "api_com.h"

pthread_mutex_t * _mutex_abo;
pthread_cond_t _client_signal = PTHREAD_COND_INITIALIZER;
pthread_cond_t _fin_signal = PTHREAD_COND_INITIALIZER;
communication * _com_abo;
pthread_t _thread_gest;
int demande_arret = 0;
int _abo_traité = 1;

void * gestionnaire(void * arg)
{

}

int initMsg()
{

    if (pthread_create(&_thread_gest, NULL, gestionnaire, NULL)!=0){
        printf("erreur creation thread1\n");
        return TECH_ERROR;
    }
    return SUCCESS;
}

int finMsg()
{
    pthread_mutex_lock(&_mutex_abo);        //lock
    demande_arret = 1;                      //demande d'arret
    pthread_cond_signal(&_client_signal);   //reveil le gestionnaire
    pthread_mutex_unlock(&_mutex_abo);      //unlock

    while(demande_arret)    //le gestionnaire passe cette variable à 0 quand il a pris la demande en compte
    {
        pthread_cond_wait(&_fin_signal, &_mutex_abo);   //on attend le signal de fin
    }

}
































int aboMsg(communication * my_com)
{
	
}
