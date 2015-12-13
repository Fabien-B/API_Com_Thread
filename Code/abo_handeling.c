#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "types_private.h"
#include "api_com.h"

#define DEBUGABOMSG
#define DEBUGDESABOMSGMSG
#define DEBUGGETNBABO
#define DEBUGISABO

int aboMsg(communication * my_com, int id)
{
#ifdef DEBUGABOMSG
    printf("\n **** \n Le thread %zu prend le mutex_abo\n **** \n ", pthread_self());
#endif
    pthread_mutex_lock(&_mutex_abo);
#ifdef DEBUGABOMSG
    printf("\n **** \n Le thread %zu à pris le mutex_abo\n **** \n ", pthread_self());
#endif
    if(service_ready==0)
    {
        pthread_mutex_unlock(&_mutex_abo);
        return NO_SERVICE;
    }
#ifdef DEBUGABOMSG
    printf("\n **** \n Le thread %zu unlock le mutex_abo \n **** \n", pthread_self());
#endif
    pthread_mutex_unlock(&_mutex_abo);
#ifdef DEBUGABOMSG
    printf("\n **** \n Le thread %zu à libérer le mutex_abo \n **** \n", pthread_self());
#endif
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

#ifdef DEBUGABOMSG
    printf("\n **** \n Le thread %zu prend le mutex client \n **** \n", pthread_self());
#endif
pthread_mutex_lock(&_mutex_clients);        //autorise une opération client
#ifdef DEBUGABOMSG
    printf("\n **** \n Le thread %zu à libérer le mutex_client \n **** \n", pthread_self());
#endif
        pthread_mutex_lock(&_mutex_abo);
        _com_abo = my_com; //mise à dispo de ma struct communication.
        _com_abo->operation = ABO;
        pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
#ifdef DEBUGABOMSG
    printf("\n **** \n Le thread %zu à envoyer un signal au gestionnaire \n **** \n", pthread_self());
#endif
        while(_com_abo->retour == -1)              //attente d'un signal venant du gestionnaire
        {
            pthread_cond_wait(_com_abo->signal_gestionnaire, &_mutex_abo);
        }
        int ret = _com_abo->retour;
        _com_abo = NULL;

#ifdef DEBUGABOMSG
    printf("\n **** \n Le thread %zu unlock le mutex_abo \n **** \n", pthread_self());
#endif
    pthread_mutex_unlock(&_mutex_abo);
#ifdef DEBUGABOMSG
    printf("\n **** \n Le thread %zu à libérer le mutex_abo \n **** \n", pthread_self());
#endif
#ifdef DEBUGABOMSG
    printf("\n **** \n Le thread %zu unlock le mutex_client \n **** \n", pthread_self());
#endif
        pthread_mutex_unlock(&_mutex_clients);
#ifdef DEBUGABOMSG
    printf("\n **** \n Le thread %zu à libérer le mutex_client \n **** \n", pthread_self());
#endif

        return ret;                   //retourne le code renvoyé par le gestionnaire

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
#ifdef DEBUGDESABOMSGMSG
    printf("\n **** \n Le thread %zu prend le mutex de sa communication\n **** \n ", pthread_self());
#endif
    pthread_mutex_lock(mycom->mutex);  //je lock mon mutex pour que le signal ne me soit envoyé que pendant mon wait
#ifdef DEBUGDESABOMSGMSG
    printf("\n **** \n Le thread %zu à pris le mutex de sa communication\n **** \n ", pthread_self());
#endif
    pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
#ifdef DEBUGDESABOMSGMSG
    printf("\n **** \n Le thread %zu à envoyer un signal au gestionnaire\n **** \n ", pthread_self());
#endif
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
#ifdef DEBUGDESABOMSGMSG
    printf("\n **** \n Le thread %zu détruit le mutex de sa communication\n **** \n ", pthread_self());
#endif
    pthread_mutex_destroy(mycom->mutex);
#ifdef DEBUGDESABOMSGMSG
    printf("\n **** \n Le thread %zu à détruit le mutex de sa communication\n **** \n ", pthread_self());
#endif
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
    int ret = my_com.retour;
    int * nb_abo = my_com.contenu;
    *nb = *nb_abo;
    free(my_com.contenu);
    pthread_mutex_unlock(&_mutex_abo);
pthread_mutex_unlock(&_mutex_clients);
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
#ifdef DEBUGISABO
    printf("\n **** \n Le thread %zu prend le mutex client fct isAbo\n **** \n", pthread_self());
#endif
    pthread_mutex_lock(&_mutex_clients);
#ifdef DEBUGISABO
    printf("\n **** \n Le thread %zu à pris le mutex client fct isAbo \n **** \n", pthread_self());
#endif
#ifdef DEBUGISABO
    printf("\n **** \n Le thread %zu prend le _mutex_abo fct isAbo\n **** \n", pthread_self());
#endif
    pthread_mutex_lock(&_mutex_abo);
#ifdef DEBUGISABO
    printf("\n **** \n Le thread %zu à pris le _mutex_abo fct isAbo \n **** \n", pthread_self());
#endif

                _com_abo = &my_com; 			//mise à dispo de ma struct communication.
                pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
#ifdef DEBUGISABO
    printf("\n **** \n Le thread %zu à envoyer un signal au gestionnaire fct isAbo\n **** \n", pthread_self());
#endif
                while(my_com.retour == -1)              //attente d'un signal venant du gestionnaire
                {
                    pthread_cond_wait(my_com.signal_gestionnaire, &_mutex_abo);   //il communique avec les objets que je lui ai spécifiés.
                }
                _com_abo = NULL;
                int ret = my_com.retour;
                *result = *data;
                free(my_com.contenu);
#ifdef DEBUGISABO
    printf("\n **** \n Le thread %zu unlock le mutex_abo fct isAbo\n **** \n", pthread_self());
#endif
    pthread_mutex_unlock(&_mutex_abo);
#ifdef DEBUGISABO
    printf("\n **** \n Le thread %zu à libérer le mutex_abo fct isAbo\n **** \n", pthread_self());
#endif
#ifdef DEBUGISABO
    printf("\n **** \n Le thread %zu unlock le mutex_client fct isAbo\n **** \n", pthread_self());
#endif
        pthread_mutex_unlock(&_mutex_clients); //opération terminée
#ifdef DEBUGISABO
    printf("\n **** \n Le thread %zu à libérer le mutex_client fct isAbo\n **** \n", pthread_self());
#endif
                return ret;                   //retourne le code renvoyé par le gestionnaire


    return TECH_ERROR;
}

