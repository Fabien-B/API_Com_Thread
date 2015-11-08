#ifndef TYPES_PRIVATE_H_INCLUDED
#define TYPES_PRIVATE_H_INCLUDED

typedef message {

    int expid;
    struct timeval *tv;     //to use with gettimeofday
    message* next;
} message;


typedef struct messagerie {
    int id;

} messagerie;


#endif // TYPES_PRIVATE_H_INCLUDED
