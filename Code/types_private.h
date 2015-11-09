#ifndef TYPES_PRIVATE_H_INCLUDED
#define TYPES_PRIVATE_H_INCLUDED
#define NB_ABO_MAX 10

typedef message {
    void * contenu;
    int expid;
    struct timeval *tv;     //to use with gettimeofday
    message* next;
} message;


typedef struct messagerie {
    int id;
    message * first_msg;
    # int nb_msg;
} messagerie;


#endif // TYPES_PRIVATE_H_INCLUDED
