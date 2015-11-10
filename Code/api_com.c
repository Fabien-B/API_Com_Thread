#include "types_private.h"
#include "api_com.h"

pthread_mutex_t _mutex_abo = PTHREAD_MUTEX_INITIALIZER;
communication * _com_abo;
int _abo_traite = 1;


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
	
	&(my_com.signal_gestionnaire) = PTHREAD_COND_INITIALIZER;
	&(my_com.mutex) = PTHREAD_MUTEX_INITIALIZER;
	
	
	int abo_possible = 0;
	while(abo_possible == 0)						// Si l'ancienne demande d'abonnement n'est pas prise en compte ont attend
		{
			pthread_mutex_lock(&_mutex_abo);
			if(_abo_traite == 1)
			{
				abo_possible = 1;
				_abo_traite = 0;
				//abonnement
			}
			
			pthread_mutex_unlock(&_mutex_abo);
			if(!abo_possible)
			{
				usleep(1000);
			}
		}
	
}
