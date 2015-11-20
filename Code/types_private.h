#ifndef TYPES_PRIVATE_H_INCLUDED
#define TYPES_PRIVATE_H_INCLUDED
#define NB_ABO_MAX 10
#include "api_com.h"

typedef struct lettre {
    message * mail;
    struct lettre * next;
} lettre;


typedef struct messagerie {
    communication * client;
    lettre * first_letter;
    // int nb_msg;
} messagerie;


extern pthread_mutex_t _mutex_abo;     //mutex utilisé pour _com_abo, _abo_traite ( et _client_signal ?)
extern pthread_cond_t _client_signal;   //condition pour sortir le gestionnaire de sa torpeur
extern communication * _com_abo;                                   //utilisé à l'abonnement
extern pthread_t * _thread_gest;                            //est à NULL si le gestionnaire n'est pas lancé
extern int _abo_traite;
extern int fin;

void * gestionnaire(void * arg);
int handleSend(messagerie * tab, int nb_messageries, int id_sender);
int handleAbo(messagerie * tab, int * nb_messageries);
int handleRcv(messagerie * mess);
int handleGetNbAbo(int nb_messageries);
int handleIsAbo(messagerie * tab, int * nb_messageries);
int close_service(int flag);
int handleDesabo(messagerie * tab, int * nb_messageries, int index_sender);
#endif // TYPES_PRIVATE_H_INCLUDED
