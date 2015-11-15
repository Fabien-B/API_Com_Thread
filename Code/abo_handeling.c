#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "types_private.h"
#include "api_com.h"


int aboMsg(communication * my_com, int id)
{

    if(_thread_gest==NULL)
    {
        return NO_SERVICE;
    }


    my_com->signal_gestionnaire = malloc(sizeof(pthread_cond_t));
    if (pthread_cond_init(my_com->signal_gestionnaire, NULL) != 0) {
        return TECH_ERROR;
    }
    my_com->mutex = malloc(sizeof(pthread_mutex_t));
    if (pthread_mutex_init(my_com->mutex,NULL) != 0) {
        return TECH_ERROR;
    }

    my_com->operation = NO_OP;
    my_com->retour = -1;

	int abo_ok = 0;
	while(abo_ok == 0)						// Si l'ancienne demande d'abonnement n'est pas prise en compte ont attend
    {
        pthread_mutex_lock(&_mutex_abo);
        if(_abo_traite == 1)    //pas d'abonnement en cours, on y va !
        {
            abo_ok = 1;         //on peut s'abonner -> pas besoin de refaire la boucle.
            _abo_traite = 0;    //indique un abonnement en cours. le gestionnaire le remettra a 1.

            //abonnement
            _com_abo = my_com; //mise à dispo de ma struct communication.

            pthread_mutex_lock(my_com->mutex);      //lock pour recevoir le signal pendant le wait (pas avant)
            pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
            pthread_mutex_unlock(&_mutex_abo);      //et libération mutex
            while(my_com->retour == -1)              //attente d'un signal venant du gestionnaire
            {
                pthread_cond_wait(my_com->signal_gestionnaire, my_com->mutex);   //il communique avec les objets que je lui ai spécifiés.
            }
            int ret = my_com->retour;
            pthread_mutex_unlock(my_com->mutex);
            return ret;                   //retourne le code renvoyé par le gestionnaire
        }

        pthread_mutex_unlock(&_mutex_abo);
        usleep(1000);
    }

    return TECH_ERROR;
}



int desaboMsg(communication * mycom)
{
    if(_thread_gest==NULL)
    {
        return NO_SERVICE;
    }

    mycom->operation = DESABO;
    mycom->retour = -1;

    pthread_mutex_lock(&_mutex_abo);
    pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
    pthread_mutex_unlock(&_mutex_abo);

    while(mycom->retour == -1)
    {
        pthread_cond_wait(mycom->signal_gestionnaire, mycom->mutex);
    }

    mycom->operation = NO_OP;
    pthread_mutex_destroy(mycom->mutex);
    return mycom->retour;
}
