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

    if (pthread_create(_thread_gest, NULL, gestionnaire, NULL)!=0){
        printf("erreur creation thread gestionnaire\n");
        return INIT_ERROR; //TECH_ERROR
    }

    while(_abo_traite == 0)     //si on arrive vite, on attend que le gestionnaire soit en régime de croisière.
    {
        pthread_mutex_lock(&_mutex_abo);    //le challenge : pouvoir prendre le mutex, et que _abo_traite soit à 1.
        pthread_mutex_unlock(&_mutex_abo);
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
    fin_com.client_id = -99;

    pthread_mutex_lock(&_mutex_abo);                                                         //sans doute inutile
    fin = 1;
    pthread_mutex_unlock(&_mutex_abo);
                                                          //sans doute inutile
    aboMsg(&fin_com);

    if(force)
    {
        fin_com.operation = CLOSESERVICE/*_FORCED*/;
    }
    else
    {
        fin_com.operation = CLOSESERVICE;
    }


    fin_com.retour = -1;

    pthread_mutex_lock(&_mutex_abo);
    pthread_mutex_lock(fin_com.mutex);  //je lock mon mutex pour que le signal ne me soit envoyé que pendant mon wait
    pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
    pthread_mutex_unlock(&_mutex_abo);      //et on le laisse bosser
    while(fin_com.retour == -1)              //tant que le gestionnaire n'a pas fait son action...
    {
        pthread_cond_wait(fin_com.signal_gestionnaire, fin_com.mutex);    //on attend
    }

    int code_retour = fin_com.retour;
    fin_com.operation = NO_OP;
    pthread_mutex_unlock(fin_com.mutex);

    return code_retour;
}
