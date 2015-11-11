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
int _abo_traite = 1;                                        //est à 0 si un abonnement est en cours

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
    ///Creation struct communication et remplissage des champs.
    communication my_com;
    my_com.client_id = -1;
        printf("%d\n",my_com.client_id);
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

            pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire

            while(my_com.retour == -1)              //attente d'un signal venant du gestionnaire
            {
                pthread_cond_wait(my_com.signal_gestionnaire, my_com.mutex);   //il communique avec les objets que je lui ai spécifiés.
            }

            return my_com.retour;                   //retourne le code renvoyé par le gestionnaire

        }

        pthread_mutex_unlock(&_mutex_abo);
        if(!abo_ok)
        {
            usleep(1000);
        }
    }

    return TECH_ERROR;
}



int aboMsg(communication * my_com)
{
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    my_com->signal_gestionnaire = &cond;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    my_com->mutex = &mutex;
    my_com->operation = ABO;
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

            pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire

            while(my_com->retour == -1)              //attente d'un signal venant du gestionnaire
            {
                pthread_cond_wait(my_com->signal_gestionnaire, my_com->mutex);   //il communique avec les objets que je lui ai spécifiés.
            }

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
