#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "types_private.h"
#include "api_com.h"
#include <string.h>

//#define DEBUGSENDMSG
//#define DEBUGRECVMSG
//#define DEBUGRECVMSGBLOCK
int sendMsg(communication * mycom, int id_dest, void * contenu, int data_size)
{
#ifdef DEBUGSENDMSG
    printf("\n **** \n %zu envoi un message\n **** \n", pthread_self());
#endif
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

    mycom->operation = SENDMSG;
    mycom->dest_id = id_dest;

    message * mess_to_send = NULL;          //création d'un message
    mess_to_send = malloc(sizeof(message));
    if(mess_to_send == NULL){
        return TECH_ERROR;
    }

    mess_to_send->expid = mycom->client_id; //signature expéditeur

    mess_to_send->contenu = NULL;           //création du contenu du message
    mess_to_send->contenu = malloc(data_size);
    if(mess_to_send->contenu == NULL){
        return TECH_ERROR;
    }

    memcpy(mess_to_send->contenu,contenu,data_size);    //copie des données (l'émétteur peut alors supprimer l'original sans affecter la réception)

    mycom->contenu = mess_to_send;
    mycom->retour = -1;

    pthread_mutex_lock(mycom->mutex);  //je lock mon mutex pour que le signal ne me soit envoyé que pendant mon wait
    pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
    while(mycom->retour == -1)              //tant que le gestionnaire n'a pas fait son action...
    {
        pthread_cond_wait(mycom->signal_gestionnaire, mycom->mutex);    //on attend
    }

    int code_retour = mycom->retour;
    mycom->operation = NO_OP;
    pthread_mutex_unlock(mycom->mutex);
    return code_retour;
}


int recvMsg(communication * mycom, message **msg)   //**msg : pointeur sur pointeur sur message -> on modifie le pointeur sur message.
{
#ifdef DEBUGRECVMSG
    printf("\n **** \n %zu reçois un message\n **** \n", pthread_self());
#endif

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

    mycom->operation = RECVMSG;
    mycom->retour = -1;

    pthread_mutex_lock(mycom->mutex);
    pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire
    while(mycom->retour == -1)
    {
        pthread_cond_wait(mycom->signal_gestionnaire, mycom->mutex);
    }

   int code_retour = mycom->retour;
    mycom->operation = NO_OP;

    if(code_retour == SUCCESS)
    {
        *msg = mycom->contenu;  //pointeur sur message vaut le pointeur retourné par le gestionnaire
    }

    pthread_mutex_unlock(mycom->mutex);
    return code_retour;
}

int recvMsgBlock(communication * mycom, message **msg)
{
#ifdef DEBUGRECVMSGBLOCK
    printf("\n **** \n %zu reçois un message block\n **** \n", pthread_self());
#endif
    int ret;
    do
    {
        ret = recvMsg(mycom,msg);
        usleep(5000);
    }while(ret == NO_MSG);

    return ret;
}



int handleSend(messagerie * tab, int nb_messageries, int id_sender)
{
    int i;
    int receiver = -1;
    for(i=0;i<nb_messageries;i++)       //recherche du destinataire
    {
        if(tab[i].client->client_id == tab[id_sender].client->dest_id)
        {
            receiver = i;
        }
    }

    if(receiver != -1)  //on a trouvé le destinataire
    {
        message * mess_to_send = tab[id_sender].client->contenu;    //récupération du message
        if(gettimeofday(&mess_to_send->tv, &mess_to_send->tz))    //datage du message
        {
                return TECH_ERROR;
        }

        lettre * new_letter = NULL;
        new_letter = malloc(sizeof(lettre));
        if(new_letter == NULL)          //vérification de la bonne marche du malloc
        {
            return TECH_ERROR;
        }
        if(mess_to_send->contenu == NULL)
        {
            printf("no content\n");
            return TECH_ERROR;
        }
        new_letter->mail = mess_to_send;    //mémorisation du message
        new_letter->next = NULL;


        if(tab[receiver].first_letter == NULL)
        {
            tab[receiver].first_letter = new_letter;
            return SUCCESS;
        }

        lettre * current_letter = tab[receiver].first_letter;
        int nb_msg = 1;
        while(current_letter->next != NULL)   //recherche pointeur lettre suivant la dernière
        {
            current_letter = current_letter->next;
            nb_msg++;
        }

        if(nb_msg >= NB_LETTER_MAX)     //boite pleine
        {
            free(mess_to_send->contenu);
            free(mess_to_send);
            free(new_letter);
            return INBOX_FULL;
        }

        current_letter->next = new_letter;
        return SUCCESS;
    }
    else    //destinataire inexistant
    {
        return ID_UNKNOWN;
    }
}


int handleRcv(messagerie * mess)
{
    if(mess->first_letter == NULL){
        return NO_MSG;
    }

    mess->client->contenu = mess->first_letter->mail;

    lettre * to_destroy = mess->first_letter;
    mess->first_letter = mess->first_letter->next;  //mise à jour du pointeur première lettre
    free(to_destroy);                               //et destruction de la vielle lettre (mais pas de son contenu)
    return SUCCESS;
}


int handleGetNbMsg(messagerie * mess)
{
	int *i = malloc(sizeof(int));
	*i = 0 ;
	mess->client->contenu = i;

	if(mess->first_letter == NULL)
	{
		return SUCCESS;
	}

	lettre * current_letter = mess->first_letter;

    (*i)++;     //ne pas oublier de compter la premiere lettre

	while(current_letter->next != NULL)   //recherche pointeur lettre suivant la dernière
	{
		current_letter = current_letter->next;
		(*i)++;
	}

	return SUCCESS;
}


int getNbMsg(communication * mycom, int* nb_msg)
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

    mycom->operation = GETNBMSG;
    mycom->retour = -1;
    mycom->contenu = NULL;

    pthread_mutex_lock(mycom->mutex);
    pthread_cond_signal(&_client_signal);   //envoie signal pour le gestionnaire

    while(mycom->retour == -1)
    {
        pthread_cond_wait(mycom->signal_gestionnaire, mycom->mutex);
    }

    int code_retour = mycom->retour;
    if(mycom->contenu != NULL)
    {
        int *nb = mycom->contenu;
        *nb_msg = *nb;
        free(mycom->contenu);
    }

    pthread_mutex_unlock(mycom->mutex);
    return code_retour;

}
