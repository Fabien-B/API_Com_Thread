#include <stdio.h>
#include "types_private.h"
#include "api_com.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

pthread_mutex_t _mutex_abo = PTHREAD_MUTEX_INITIALIZER;     //mutex utilisé pour _com_abo, _abo_traite ( et _client_signal ?)
pthread_cond_t _client_signal = PTHREAD_COND_INITIALIZER;   //condition pour sortir le gestionnaire de sa torpeur
communication * _com_abo;                                   //utilisé à l'abonnement
pthread_t * _thread_gest = NULL;                            //est à NULL si le gestionnaire n'est pas lancé
int _abo_traite = 0;                                        //est à 0 si un abonnement est en cours, ou le lancement du gestionnaire
int fin = 0;

void * gestionnaire(void * arg)
{
    pthread_mutex_lock(&_mutex_abo);    //lock mutex pour ne rien faire tant qu'on est pas en croisière

    int nb_messageries = 0;
    messagerie tab[NB_ABO_MAX+1];     //crée les messageries

    _abo_traite = 1;                //variable pour la vérification de la phase croisière
    pthread_cond_wait(&_client_signal, &_mutex_abo);      //attente d'un signal (avec unlock automatique du mutex, puis lock à la sortie)
    while(1)
    {
    usleep(1000);
    if(_com_abo != NULL && _com_abo->operation != NO_OP)
    {
        //ACTIONS (switch case)!
        int ret = -1;
        switch(_com_abo->operation)
        {
            case ABO:
                ret = handleAbo(tab, &nb_messageries);
                break;
            case GETNBABO:
                ret = handleGetNbAbo(nb_messageries);
                break;
            case ISABO:
                ret = handleIsAbo(tab, &nb_messageries);
                break;
        }
        _com_abo->retour = ret;
        pthread_cond_signal(_com_abo->signal_gestionnaire);
    }

    int i;
    for(i=0;i<nb_messageries;i++) //parcours liste abo executer les actions id demandées.
    {
        pthread_mutex_lock(tab[i].client->mutex);
        if(tab[i].client->operation != NO_OP)
        {
            //ACTIONS (switch case)!
            int ret = -1;
            switch(tab[i].client->operation)
            {
                case SENDMSG:
                    ret = handleSend(tab, nb_messageries, i);
                    break;
                case RECVMSG:
                    ret = handleRcv(&tab[i]);
                    break;
                case CLOSESERVICE:
                    ret = close_service(0);
                    break;
                case CLOSESERVICE_FORCED:
                    ret = close_service(1);
                    break;
            }
            tab[i].client->retour = ret;
            pthread_cond_signal(tab[i].client->signal_gestionnaire);
        }
        pthread_mutex_unlock(tab[i].client->mutex);
        if (fin)
        {
            fin = 0;
            printf("pthread_exit ici\n");
            //pthread_exit(0);
        }
    }
    pthread_cond_wait(&_client_signal, &_mutex_abo);      //attente d'un signal pour effectuer la boule suivante
    }
    pthread_exit(0);
}


int close_service(int flag)
{
    ///TODO
    //frees....

    return 789;
}

