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

int handleAbo(messagerie * tab, int * nb_messageries)
{
        _com_abo->operation = NO_OP;
        int flag = SUCCESS;     //succes by default, modified if there is an error.
        if(*nb_messageries<NB_ABO_MAX || fin)
        {
            int i;
            for(i=0;i<*nb_messageries;i++) //parcours liste abo pour vérif id demandé.
            {
                pthread_mutex_lock(tab[i].client->mutex);

                if(tab[i].client->client_id == _com_abo->client_id)
                {
                    flag = ID_IN_USE;
                }
                pthread_mutex_unlock(tab[i].client->mutex);
            }

            if(flag != ID_IN_USE)   //si id dispo, on enregistre.
            {
                tab[*nb_messageries].client = _com_abo;  //tout ok, stockage dans le tableau
                tab[*nb_messageries].first_letter = NULL;   //pas de msg au début
                (*nb_messageries)++;  //un abo de plus.
            }
        }
        else
        {
            flag = MAX_ABO;
        }
        _abo_traite = 1;    //demande d'abo traitée, on peut en faire d'autre !
        return flag;
}


int handleDesabo(messagerie * tab, int * nb_messageries, int index_sender)
{
    if(tab[index_sender].first_letter != NULL)      //liberation des lettres
    {
        lettre * current_letter = tab[index_sender].first_letter;
        while(current_letter->next != NULL)   //recherche pointeur lettre suivant la dernière
        {
            lettre * next_letter = current_letter->next;
            free(current_letter->mail);
            free(current_letter->next);
            free(current_letter);
            current_letter = next_letter;
        }
            free(current_letter->mail);
            free(current_letter->next);
            free(current_letter);
    }

    int i;
    for(i=index_sender;i<*nb_messageries-1;i++)     //decalage des messageries suivantes
    {
        tab[i]=tab[i+1];
    }
    (*nb_messageries)--;
    return SUCCESS;
}


int handleGetNbAbo(int nb_messageries)
{
        _com_abo->operation = NO_OP;
        int * contenu = _com_abo->contenu;  //mémoire allouée dans GetNbAbo du thread client.
        *contenu = nb_messageries;
        return SUCCESS;             //pourquoi cette fonction se passerait-elle mal ?
}

int handleIsAbo(messagerie * tab, int * nb_messageries)
{
    _com_abo->operation = NO_OP;
    int result = 0;
    int i;
    int * data = _com_abo->contenu;
    for(i=0;i<*nb_messageries;i++) //parcours liste abo pour vérif id demandé.
    {
        pthread_mutex_lock(tab[i].client->mutex);       //est-ce bien utile ? Ce n'est que de la lecture...
        if(tab[i].client->client_id == *data)
        {
            result = 1;
        }
        pthread_mutex_unlock(tab[i].client->mutex);
    }
    *data = result;
    return SUCCESS;
}
