#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "types_private.h"
#include "api_com.h"


int aboMsg(communication * my_com, int id)
{
    printf("\n **** \n le thread %zu s'abonne \n **** \n",pthread_self());
    pthread_mutex_lock(&_mutex_abo);
    if(service_ready==0)
    {
        pthread_mutex_unlock(&_mutex_abo);
        return NO_SERVICE;
    }
    pthread_mutex_unlock(&_mutex_abo);

    int abo_state = 0;
    //isAbo(id,&abo_state);
    if(abo_state)
    {
        return ID_IN_USE;
    }

    //création et initialisation des mutex et condition. ---------------------------
    my_com->signal_gestionnaire = malloc(sizeof(pthread_cond_t));
    if (pthread_cond_init(my_com->signal_gestionnaire, NULL) != 0) {
        return TECH_ERROR;
    }
    my_com->mutex = NULL;
    my_com->mutex = malloc(sizeof(pthread_mutex_t));
    if (pthread_mutex_init(my_com->mutex,NULL) != 0) {
        return TECH_ERROR;
    }
    //-------------------------------------------------------------------------------

    my_com->client_id = id;
    my_com->retour = -1;

	int abo_ok = 0;
	while(abo_ok == 0)						// Si l'ancienne demande d'abonnement n'est pas prise en compte ont attend
    {
        printf("prise du mutex_abo \n");
        pthread_mutex_lock(&_mutex_abo);
        printf("mutex_abo pris par %zu\n", pthread_self());
        if(_abo_traite == 1)    //pas d'abonnement en cours, on y va !
        {
printf("prise du mutex_clients\n");
pthread_mutex_lock(&_mutex_clients);
printf("mutex_clients pris par le thread %zu\n", pthread_self());
            abo_ok = 1;         //on peut s'abonner -> pas besoin de refaire la boucle.
            _abo_traite = 0;    //indique un abonnement en cours. le gestionnaire le remettra a 1.
            //abonnement
            _com_abo = my_com; //mise à dispo de ma struct communication.
            _com_abo->operation = ABO;
            pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
            while(_com_abo->retour == -1)              //attente d'un signal venant du gestionnaire
            {
                pthread_cond_wait(_com_abo->signal_gestionnaire, &_mutex_abo);
            }
            int ret = _com_abo->retour;
            _com_abo = NULL;
pthread_mutex_unlock(&_mutex_clients);
            pthread_mutex_unlock(&_mutex_abo);
            return ret;                   //retourne le code renvoyé par le gestionnaire
        }
        pthread_mutex_unlock(&_mutex_abo);
        usleep(1000);
    }

    return TECH_ERROR;
}



int desaboMsg(communication * mycom)
{
    if(service_ready==0)
    {
        return NO_SERVICE;
    }

    int abo_state;
    isAbo(mycom->client_id,&abo_state);
    if(!abo_state)
    {
        return NO_ABO;
    }

    mycom->operation = DESABO;
    mycom->retour = -1;

    pthread_mutex_lock(mycom->mutex);  //je lock mon mutex pour que le signal ne me soit envoyé que pendant mon wait
    pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire

    while(mycom->retour == -1)
    {
        pthread_cond_wait(mycom->signal_gestionnaire, mycom->mutex);
    }

    mycom->operation = NO_OP;       //inutile en theorie
    //int destr = pthread_mutex_destroy(mycom->mutex);              //ça plante: pourquoi ?
    int destr = pthread_cond_destroy(mycom->signal_gestionnaire);
    if(destr)
    {
        return TECH_ERROR;
    }

    pthread_mutex_destroy(mycom->mutex);
    //pthread_cond_destroy(mycom->signal_gestionnaire);
    free(mycom->mutex);
    free(mycom->signal_gestionnaire);
    //*contenu est géré par l'utilisateur
mycom->operation = NO_OP;
    return mycom->retour;
}


int getNbAbo(int * nb)
{
    if(service_ready==0)
    {
        return NO_SERVICE;
    }

    communication my_com;       //création d'une struct communication pour communiquer avec le gestionnaire

    //création et initialisation des mutex et condition. ---------------------------
    my_com.signal_gestionnaire = malloc(sizeof(pthread_cond_t));
    if (pthread_cond_init(my_com.signal_gestionnaire, NULL) != 0) {
        return TECH_ERROR;
    }
    my_com.mutex = malloc(sizeof(pthread_mutex_t));
    if (pthread_mutex_init(my_com.mutex,NULL) != 0) {
        return TECH_ERROR;
    }
    //-------------------------------------------------------------------------------

    my_com.client_id = -1;          //sert à rien ici
    my_com.operation = GETNBABO;
    my_com.contenu = NULL;
    my_com.contenu = malloc(sizeof(int));
    if(my_com.contenu == NULL)
    {
        return TECH_ERROR;
    }
    my_com.retour = -1;

    pthread_mutex_lock(&_mutex_clients);
    pthread_mutex_lock(&_mutex_abo);
    _com_abo = &my_com; //mise à dispo de ma struct communication.
    pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
    while(my_com.retour == -1)              //attente d'un signal venant du gestionnaire
    {
        pthread_cond_wait(my_com.signal_gestionnaire, &_mutex_abo);   //il communique avec les objets que je lui ai spécifiés.
    }
    _com_abo = NULL;
    pthread_mutex_unlock(&_mutex_clients);
    int ret = my_com.retour;
    int * nb_abo = my_com.contenu;
    *nb = *nb_abo;
    free(my_com.contenu);
    pthread_mutex_unlock(&_mutex_abo);
    return ret;                   //retourne le code renvoyé par le gestionnaire
}


int isAbo(int id, int * result)
{
    if(service_ready==0)
    {
        return NO_SERVICE;
    }

    communication my_com;       //création d'une struct communication pour communiquer avec le gestionnaire

    //création et initialisation des mutex et condition. ---------------------------
    my_com.signal_gestionnaire = malloc(sizeof(pthread_cond_t));
    if (pthread_cond_init(my_com.signal_gestionnaire, NULL) != 0) {
        return TECH_ERROR;
    }
    my_com.mutex = malloc(sizeof(pthread_mutex_t));
    if (pthread_mutex_init(my_com.mutex,NULL) != 0) {
        return TECH_ERROR;
    }
    //-------------------------------------------------------------------------------

    my_com.client_id = -1;          //sert à rien ici
    my_com.operation = ISABO;
    my_com.contenu = NULL;
    my_com.contenu = malloc(sizeof(int));
    if(my_com.contenu == NULL)
    {
        return TECH_ERROR;
    }
    int * data = my_com.contenu;
    *data = id;
    my_com.retour = -1;

    pthread_mutex_lock(&_mutex_clients);
    pthread_mutex_lock(&_mutex_abo);

    if(_com_abo!=NULL)
    {
        return TECH_ERROR;
    }
    _com_abo = &my_com; 			//mise à dispo de ma struct communication.
    pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
    while(my_com.retour == -1)              //attente d'un signal venant du gestionnaire
    {
        pthread_cond_wait(my_com.signal_gestionnaire, &_mutex_abo);   //il communique avec les objets que je lui ai spécifiés.
    }
    _com_abo = NULL;
    pthread_mutex_unlock(&_mutex_clients);
    int ret = my_com.retour;
    *result = *data;
    free(my_com.contenu);
    pthread_mutex_unlock(&_mutex_abo);
    return ret;                   //retourne le code renvoyé par le gestionnaire
}

