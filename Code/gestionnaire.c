#include <stdio.h>
#include "types_private.h"
#include "api_com.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t _mutex_abo = PTHREAD_MUTEX_INITIALIZER;     //mutex utilisé pour _com_abo, _abo_traite ( et _client_signal ?)
pthread_cond_t _client_signal = PTHREAD_COND_INITIALIZER;   //condition pour sortir le gestionnaire de sa torpeur
communication * _com_abo;                                   //utilisé à l'abonnement
pthread_t * _thread_gest = NULL;                            //est à NULL si le gestionnaire n'est pas lancé
int _abo_traite = 0;                                        //est à 0 si un abonnement est en cours, ou le lancement du gestionnaire

void * gestionnaire(void * arg)
{
    pthread_mutex_lock(&_mutex_abo);    //lock mutex pour ne rien faire tant qu'on est pas en croisière

    int nb_messageries = 0;
    messagerie tab[NB_ABO_MAX];     //crée les messageries
    _abo_traite = 1;                //variable pour la vérification de la phase croisière
    pthread_mutex_unlock(&_mutex_abo);
    pthread_cond_wait(&_client_signal, &_mutex_abo);      //attente d'un signal (avec unlock automatique du mutex, puis lock à la sortie)
    while(1)
    {
        if(!_abo_traite)    //un abonnement à gérer !
        {
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
            _abo_traite = 1;    //demande d'abo traitée, on peut en faire d'autre !
        }

        int i;
        for(i=0;i<nb_messageries;i++) //parcours liste abo executer les actions id demandées.
        {
            //ACTIONS (switch case)!
        }

        pthread_cond_wait(&_client_signal, &_mutex_abo);      //attente d'un signal pour effectuer la boule suivante
    }
    pthread_exit(0);
}

