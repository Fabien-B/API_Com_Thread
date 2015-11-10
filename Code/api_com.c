#include "types_private.h"
#include "api_com.h"

pthread_mutex_t * _mutex_abo = PTHREAD_MUTEX_INITIALIZER;
communication * _com_abo;
int _abo_traité = 1;


void * gestionnaire(void * arg)
{

}

int initMsg()
{
    pthread_t thread_gest;
    if (pthread_create(&idThread1, NULL, foncThread1, NULL)!=0){
        printf("erreur creation thread1\n");
        exit(1);
    }
}










































int aboMsg(communication * my_com)
{
	
}
