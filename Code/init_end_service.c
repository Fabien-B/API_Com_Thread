#define _REENTRANT
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "types_private.h"
#include "api_com.h"


int initMsg()
{
    pthread_mutex_lock(&_mutex_abo);
    int* serv_ready = &service_ready;

    if(_thread_gest != NULL) // != NULL : déja lancé
    {
        int ok=0;
        while(!ok)
        {
            pthread_mutex_unlock(&_mutex_abo);
            usleep(10);     //service en cours de lancement dans un autre thread, on attend qu'il soit vraiment lancé...
            pthread_mutex_lock(&_mutex_abo);
            ok = service_ready;
        }
        pthread_mutex_unlock(&_mutex_abo);
        return ALREADY_LAUNCH;
    }


    _thread_gest = malloc(sizeof(pthread_t));


    if (pthread_create(_thread_gest, PTHREAD_CREATE_JOINABLE, gestionnaire, NULL)!=0){
        printf("erreur creation thread gestionnaire\n");
        return INIT_ERROR; //TECH_ERROR
    }

    while(_abo_traite == 0)     //si on arrive vite, on attend que le gestionnaire soit en régime de croisière.
    {
        pthread_mutex_unlock(&_mutex_abo);
        usleep(1000);
        pthread_mutex_lock(&_mutex_abo);    //le challenge : pouvoir prendre le mutex, et que _abo_traite soit à 1.

    }

    service_ready = 1;      //le service est maintenant lancé ! (il attend dans le wait)

    pthread_mutex_unlock(&_mutex_abo);
    return SUCCESS;
}



int finMsg(int force)
{
    if(service_ready==0)
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

pthread_mutex_lock(&_mutex_clients);
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
    service_ready=0;
    pthread_mutex_unlock(&_mutex_abo);

    //pthread_join(*_thread_gest,NULL);
    _thread_gest = NULL;
    service_ready = 0;
    fin = 0;

    pthread_mutex_unlock(&_mutex_clients);
    return ret;                 //retourne le code renvoyé par le gestionnaire

}
