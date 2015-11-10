#include <stdio.h>
#include "types_private.h"
#include "api_com.h"

pthread_mutex_t * mutex_abo;
communication * com_abo;
pthread_t _thread_gest;

void * gestionnaire(void * arg)
{

}

int initMsg()
{

    if (pthread_create(&_thread_gest, NULL, gestionnaire, NULL)!=0){
        printf("erreur creation thread1\n");
        return TECH_ERROR;
    }
    return SUCCESS;
}

int finMsg()
{

}
































int aboMsg(communication *)
{
	
}
