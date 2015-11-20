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
        usleep(1000);
        pthread_mutex_lock(&_mutex_abo);
        pthread_mutex_unlock(&_mutex_abo);    //le challenge : pouvoir prendre le mutex, et que _abo_traite soit à 1.

    }
    return SUCCESS;
}




int finMsg(int force)
{
    if(_thread_gest==NULL)
    {
        return NO_SERVICE;
    }

    communication my_com;       //création d'une struct communication pour communiquer avec le gestionnaire

    //création et initialisation des mutex et condition. ---------------------------
    my_com.signal_gestionnaire = malloc(sizeof(pthread_cond_t));
    if (pthread_cond_init(my_com.signal_gestionnaire, NULL) != 0) {
        return TECH_ERROR;
    }
    my_com.mutex = malloc(sizeof(pthread_mutex_t));
    if (pthread_mutex_init(my_com.mutex,NULL) != 0) {
        return TECH_ERROR;
    }
    //-------------------------------------------------------------------------------

    my_com.client_id = -1;          //sert à rien ici
    if(force)
    {
        my_com.operation = CLOSESERVICE_FORCED;
    }
    else
    {
        my_com.operation = CLOSESERVICE;
    }
    my_com.retour = -1;

    pthread_mutex_lock(&_mutex_abo);
    fin = 1;
    _com_abo = &my_com; //mise à dispo de ma struct communication.
    pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
    while(my_com.retour == -1)              //attente d'un signal venant du gestionnaire
    {
        pthread_cond_wait(my_com.signal_gestionnaire, &_mutex_abo);   //il communique avec les objets que je lui ai spécifiés.
    }
    _com_abo = NULL;
    int ret = my_com.retour;
    my_com.operation = NO_OP;
    pthread_mutex_unlock(&_mutex_abo);
    return ret;                 //retourne le code renvoyé par le gestionnaire

    ///TODO : pthread_join
//pthread_join(*_thread_gest,NULL);
    //int pj;
   /* if(pthread_join(*_thread_gest,NULL))
    {
        return pj;
    }*/
}
