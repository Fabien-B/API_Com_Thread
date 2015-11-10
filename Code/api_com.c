#include "types_private.h"
#include "api_com.h"

pthread_mutex_t * mutex_abo;
communication * com_abo;

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









































