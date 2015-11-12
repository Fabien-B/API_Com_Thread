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




int finMsg()
{
    if(_thread_gest==NULL)  //thread non lancé, pas besoinde l'arreter.
    {
        return NO_SERVICE;
    }

    ///Creation struct communication et remplissage des champs.
    communication my_com;
    my_com.client_id = -1;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    my_com.signal_gestionnaire = &cond;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    my_com.mutex = &mutex;
    my_com.operation = CLOSESERVICE;
    my_com.retour = -1;

	int abo_ok = 0;

	while(abo_ok == 0)						// Si l'ancienne demande d'abonnement n'est pas prise en compte ont attend
    {
        pthread_mutex_lock(&_mutex_abo);

        if(_abo_traite == 1)    //pas d'abonnement en cours, on y va !
        {
            abo_ok = 1;         //on peut s'abonner -> pas besoin de refaire la boucle.
            _abo_traite = 0;    //indique un abonnement en cours. le gestionnaire le remettra a 1.

            //abonnement
            _com_abo = &my_com; //mise à dispo de ma struct communication.
printf("stop\n");
sleep(2);
            pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
            pthread_mutex_unlock(&_mutex_abo);
            //int ret = my_com.retour;
            while(_com_abo->retour == -1)              //attente d'un signal venant du gestionnaire
            {
                pthread_cond_wait(_com_abo->signal_gestionnaire, _com_abo->mutex);   //il communique avec les objets que je lui ai spécifiés.
            }

            pthread_mutex_unlock(&_mutex_abo);
            int ret = _com_abo->retour;
            return ret;                   //retourne le code renvoyé par le gestionnaire

        }

        pthread_mutex_unlock(&_mutex_abo);
        if(!abo_ok)
        {
            usleep(1000);
        }
    }

    return TECH_ERROR;
}
