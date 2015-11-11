#ifndef TYPES_PRIVATE_H_INCLUDED
#define TYPES_PRIVATE_H_INCLUDED
#define NB_ABO_MAX 10
#include "api_com.h"

typedef struct message {
    void * contenu;
    int expid;
    struct timeval *tv;     //to use with gettimeofday
    struct message * next;
} message;


typedef struct messagerie {
    communication * client;
    message * first_msg;
    // int nb_msg;
} messagerie;


#endif // TYPES_PRIVATE_H_INCLUDED
