#include <stdio.h>
#include "types_private.h"
#include "api_com.h"


pthread_mutex_t _mutex_abo = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t _client_signal = PTHREAD_COND_INITIALIZER;
pthread_cond_t _fin_signal = PTHREAD_COND_INITIALIZER;
communication * _com_abo;
int demande_arret = 0;
int _abo_traité = 1;
pthread_t * _thread_gest = NULL;    //à NULL si le gestionnaire n'est pas lancé
int _abo_traite = 1;

void * gestionnaire(void * arg)
{

}

int initMsg()
{
    if(_thread_gest) // != NULL : déja lancé
    {
        return ALREADY_LAUNCH;
    }

    pthread_t thread_gestionnaire;          //on déclare un pthread_t
    _thread_gest = &thread_gestionnaire;    //et on le donne a notre variable globale

    if (pthread_create(_thread_gest, NULL, gestionnaire, NULL)!=0){
        printf("erreur creation thread gestionnaire\n");
        return INIT_ERROR; //TECH_ERROR
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
	
	&(my_com.signal_gestionnaire) = PTHREAD_COND_INITIALIZER;
	&(my_com.mutex) = PTHREAD_MUTEX_INITIALIZER;
	
	
	int abo_possible = 0;
	while(abo_possible == 0)						// Si l'ancienne demande d'abonnement n'est pas prise en compte ont attend
		{
			pthread_mutex_lock(&_mutex_abo);
			if(_abo_traite == 1)
			{
				abo_possible = 1;
				_abo_traite = 0;
				//abonnement
			}
			
			pthread_mutex_unlock(&_mutex_abo);
			if(!abo_possible)
			{
				usleep(1000);
			}
		}
	
}
