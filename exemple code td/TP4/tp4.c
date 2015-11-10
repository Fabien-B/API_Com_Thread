// DZIECIOL Nicolas tp4

#define _REENTRANT
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


sem_t _nbLibrEntree;
sem_t _nbVoit;

void * fonc_entree(void * arg)
{
	while(1)
	{
		sleep(rand()%7);	// attente voiture entree
		

		//P(nbLibrEntree):
		if(sem_wait(&_nbLibrEntree)==-1)		// decrementation du nb de place libre pour entree
		{
			perror("Entree:erreur wait:");
			pthread_exit(NULL);
		}

		//V(nbVoit):
		if(sem_post(&_nbVoit)==-1)		// increment du nb de voitures
		{
			perror("Entree:erreur post:");
			pthread_exit(NULL);
		}
		printf("Entrée :\n");
	}
}

void * fonc_sortie(void * arg)
{	 
	while(1)
	{
		sleep(rand()%7);	// attente voiture sortie
		
		
		//P(nbVoit):
		if(sem_wait(&_nbVoit)==-1)		// increment du nb de place libre pour entrer
		{
			perror("Sortie:erreur post:");
			pthread_exit(NULL);
		}

		//V(nbLibrEntree):
		if(sem_post(&_nbLibrEntree)==-1)		// decrementation du nb de voiture
		{
			perror("Sortie:erreur wait:");
			pthread_exit(NULL);
		}
		printf("Sortie :\n");

	}
}



int main(void)
{
	pthread_t threadEntree, threadSortie;


	if(sem_init(&_nbLibrEntree,0,30)==1)			// initialisation du semaphore du nombre de place libre pour entrer à 30
	{
		perror("init semaphore nbLibrEntree");
		exit(1);
	}

	if(sem_init(&_nbVoit,0,0)==1)			// initialisation du semaphore du nombre de voitures à 0	
	{
		perror("init semaphore nbVoit");
		exit(1);
	}

	if(pthread_create(&threadEntree,NULL,fonc_entree,NULL)!=0)	// creation thread entrée
	{
		perror("creation thread entrée");
		exit(1);
	}

	if(pthread_create(&threadSortie,NULL,fonc_sortie,NULL)!=0)	// creation thread sortir
	{
		perror("creation thread entrée");
		exit(1);
	}

	pthread_join(threadEntree,NULL);
	pthread_join(threadSortie,NULL);

	sem_destroy(&_nbLibrEntree);
	sem_destroy(&_nbVoit);
	return 0;

}
