#include <stdio.h>
#include "types_private.h"
#include "api_com.h"


pthread_mutex_t _mutex_abo = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t _client_signal = PTHREAD_COND_INITIALIZER;
communication * _com_abo;
int demande_arret = 0;
pthread_t * _thread_gest = NULL;    //à NULL si le gestionnaire n'est pas lancé
int _abo_traite = 1;

void * gestionnaire(void * arg)
{
    int nb_messageries = 0;
    messagerie tab[NB_ABO_MAX];     //crée les messageries

    while(1)
    {
        //pthread_mutex_lock(&_mutex_abo);
        pthread_cond_wait(&_client_signal, &_mutex_abo);      //attente d'un signal

        if(!_abo_traite)    //un abonnement à gérer !
        {
            //pthread_mutex_lock(&_mutex_abo);  // fait automatiquement à la sortie du wait.
            int flag = SUCCESS;     //succes by default, modified if there is an error.

            if(nb_messageries<NB_ABO_MAX)
            {
                int i;
                for(i=0;i<nb_messageries;i++) //parcours liste abo pour vérif id demandé.
                {
                    if(tab[i].client->client_id == _com_abo->client_id)
                    {
                        flag = ID_IN_USE;
                    }
                }

                tab[nb_messageries].client = _com_abo;  //tout ok, stockage dans le tableau
                tab[nb_messageries].first_msg = NULL;   //pas de msg au début
                nb_messageries ++;  //un abo de plus.
            }
            else
            {
                flag = MAX_ABO;
            }

            pthread_mutex_lock(_com_abo->mutex);  //lock mutex client
            _com_abo->retour = flag;       //met la valeur de retour à success.
            pthread_cond_signal(_com_abo->signal_gestionnaire);   //réveille le client.
            pthread_mutex_unlock(_com_abo->mutex);    //unlock mutex client

            _abo_traite = 1;    //abo traitée, on peut en faire d'autre !
            pthread_mutex_unlock(&_mutex_abo); //on rend la main.
        }

        int i;
        for(i=0;i<nb_messageries;i++) //parcours liste abo pour vérif id demandé.
        {
            //ACTIONS (switch case)!
        }
    }
    pthread_exit(0);
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
