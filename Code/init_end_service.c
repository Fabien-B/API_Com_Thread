#define _REENTRANT
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "types_private.h"
#include "api_com.h"


int initMsg()
{
    if(_thread_gest != NULL) // != NULL : déja lancé
    {
        return ALREADY_LAUNCH;
    }

    pthread_t thread_gestionnaire;          //on déclare un pthread_t
    _thread_gest = &thread_gestionnaire;    //et on le donne a notre variable globale

    pthread_mutex_lock(&_mutex_abo);

    //if (pthread_create(_thread_gest, NULL, gestionnaire, NULL)!=0){
    if (pthread_create(_thread_gest, PTHREAD_CREATE_JOINABLE, gestionnaire, NULL)!=0){
        printf("erreur creation thread gestionnaire\n");
        return INIT_ERROR; //TECH_ERROR
    }
    pthread_mutex_unlock(&_mutex_abo);

    while(_abo_traite == 0)     //si on arrive vite, on attend que le gestionnaire soit en régime de croisière.
    {
        //usleep(1000);
        pthread_mutex_lock(&_mutex_abo);
        pthread_mutex_unlock(&_mutex_abo);    //le challenge : pouvoir prendre le mutex, et que _abo_traite soit à 1.

    }
    return SUCCESS;
}




int finMsg(int force)
{
    if(_thread_gest==NULL)  //thread non lancé, pas besoinde l'arreter.
    {
        return NO_SERVICE;
    }

    communication fin_com;

    pthread_mutex_lock(&_mutex_abo);
    fin = 1;
    pthread_mutex_unlock(&_mutex_abo);

    aboMsg(&fin_com, 45);

    pthread_mutex_lock(fin_com.mutex);

//sleep(1);
    if(force)
    {
        fin_com.operation = CLOSESERVICE_FORCED;
    }
    else
    {
        fin_com.operation = CLOSESERVICE;
    }

    fin_com.retour = -1;

    pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
//sleep(2);

    while(fin_com.retour == -1)              //tant que le gestionnaire n'a pas fait son action...
    //while(1)              //tant que le gestionnaire n'a pas fait son action...
    {
    usleep(1000);
        pthread_cond_wait(fin_com.signal_gestionnaire, fin_com.mutex);    //on attend
    }

    int code_retour = fin_com.retour;
    fin_com.operation = NO_OP;
    pthread_mutex_unlock(fin_com.mutex);
    //pthread_join(*_thread_gest,NULL);
    //int pj;
   /* if(pthread_join(*_thread_gest,NULL))
    {
        return pj;
    }*/
    return code_retour;
}
