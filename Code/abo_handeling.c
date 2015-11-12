#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "types_private.h"
#include "api_com.h"


int aboMsg(communication * my_com)
{

    if(_thread_gest==NULL)
    {
        return NO_SERVICE;
    }

    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    my_com->signal_gestionnaire = &cond;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    my_com->mutex = &mutex;
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
            pthread_mutex_unlock(my_com->mutex);
            return my_com->retour;                   //retourne le code renvoyé par le gestionnaire

        }

        pthread_mutex_unlock(&_mutex_abo);
        if(!abo_ok)
        {
            usleep(1000);
        }
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
    return mycom->retour;
}
