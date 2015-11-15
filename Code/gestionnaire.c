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
        //_mutex_abo locké par le wait
        if(!_abo_traite)    //un abonnement à gérer !
        {
printf("ABO\n");
            int flag = SUCCESS;     //succes by default, modified if there is an error.
            pthread_mutex_lock(_com_abo->mutex);  //lock mutex client
            if(nb_messageries<NB_ABO_MAX || fin)
            {
                int i;
                for(i=0;i<nb_messageries;i++) //parcours liste abo pour vérif id demandé.
                {
                    pthread_mutex_lock(tab[i].client->mutex);
                    //pthread_mutex_lock(????????????????????????|||||????);
                    if(tab[i].client->client_id == _com_abo->client_id)
                    {
                        flag = ID_IN_USE;
                    }
                    pthread_mutex_unlock(tab[i].client->mutex);
                }

                if(flag != ID_IN_USE)   //si id dispo, on enregistre.
                {
                    tab[nb_messageries].client = _com_abo;  //tout ok, stockage dans le tableau
                    tab[nb_messageries].first_letter = NULL;   //pas de msg au début
                    nb_messageries ++;  //un abo de plus.
                }
            }
            else
            {
                flag = MAX_ABO;
            }
            _com_abo->retour = flag;       //met la valeur de retour à success.
            pthread_cond_signal(_com_abo->signal_gestionnaire);   //réveille le client.
            pthread_mutex_unlock(_com_abo->mutex);    //unlock mutex client
            _abo_traite = 1;    //demande d'abo traitée, on peut en faire d'autre !
            _com_abo = NULL;
        }


        int i;
        for(i=0;i<nb_messageries;i++) //parcours liste abo executer les actions id demandées.
        {
//usleep(1000);
//printf("NB MSGR : %d (%d)\n", nb_messageries,i);
printf("avant lock %d\n",i);
//sleep(5);
            pthread_mutex_lock(tab[i].client->mutex);
printf("APRES lock %d\n",i);
            if(tab[i].client->operation != NO_OP)
            {
                //ACTIONS (switch case)!
                //int ret;
                printf("AAAAAAAAAAAAAAAA");
                int ret = -1;
                switch(tab[i].client->operation)
                {
                    case SENDMSG:
                        ret = handleSend(tab[i].client->client_id, tab[i].client->contenu);
                        break;
                    case RECVMSG:
                        //ret = handleRcv();
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
printf("AV\n");
            pthread_mutex_unlock(tab[i].client->mutex);
            if (fin)
            {
                sleep(2);
                //pthread_exit(0);
            }
printf("AP -> boucle OK\n");
        }

        pthread_cond_wait(&_client_signal, &_mutex_abo);      //attente d'un signal pour effectuer la boule suivante
    }
    pthread_exit(0);
}



int handleSend(int sender_id, void * contenu)   //crée un message daté et signé avec les données du client.
{
    printf("ON ENVOIE!!!\n");
    return 0;
}

int handleRcv()
{
    return 2;
}

int close_service(int flag)
{
    //frees....

    return 789;
}
