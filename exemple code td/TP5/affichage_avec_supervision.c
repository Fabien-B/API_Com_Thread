// DZIECIOL Nicolas tp5 serveur d'affichage avec supervision

#define _TAILLEBUF 50
#define _TAILLEMESS 30
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

char _buffer[_TAILLEBUF][_TAILLEMESS];
int _nbmess = 0;
int _arret_lecture=0;
int _arret_tot = 0;
pthread_cond_t _full_buff = PTHREAD_COND_INITIALIZER;
pthread_cond_t _empty_buff = PTHREAD_COND_INITIALIZER;
pthread_mutex_t _mut = PTHREAD_MUTEX_INITIALIZER;


void * Ecriture(void * arg)
{
	int index_ecriture = 0;	
	while(1)
	{
		if(_arret_tot)								// Si on demande l'arret du programme
		{	
			printf("Fin tâche Ecriture\n");
			pthread_exit(0);
		}

		pthread_mutex_lock(&_mut);						// Lock du Mutex

		while(_nbmess == _TAILLEBUF)						// On ecrit plus si le buffer est plein 
		{
			pthread_cond_wait(&_full_buff, &_mut);
		}

		if(_arret_tot)								// Si on demande l'arret du programme
		{	
			printf("Fin tâche Ecriture\n");
			pthread_exit(0);
		}

		sprintf(_buffer[index_ecriture],"il y a %d voitures",index_ecriture); 	// création du message a afficher
		_nbmess ++;								// Maj du nbmess
		index_ecriture = (index_ecriture+1)%_TAILLEBUF;				// modification de l'index
		pthread_cond_signal(&_empty_buff);					// Envoi d'un signal pour dire que le buffer n'est pas vide
		pthread_mutex_unlock(&_mut);						// unlock du mutex
		sleep(3);
	}
}

void * Lecture(void * arg)
{	
	int index_lecture = 0;	
	while(1)
	{
		if(_arret_tot && _nbmess == 0)						// Si on demande l'arret du programme
		{	
			printf("Fin tâche Lecture\n");
			pthread_exit(0);
		}

		pthread_mutex_lock(&_mut);						// Lock du Mutex
		while((_nbmess == 0 || _arret_lecture == 1) && !_arret_tot) 		// On ne lit plus s'il ny a pas de message 
		{
			pthread_cond_wait(&_empty_buff, &_mut);
		}
		
		if(_arret_tot && _nbmess == 0)						// Si on demande l'arret du programme
		{	
			printf("Fin tâche Lecture\n");
			pthread_exit(0);
		}
		if( _nbmess != 0)
		{
			printf("%s\n",_buffer[index_lecture]);				// affichage du message
			index_lecture = (index_lecture+1)%_TAILLEBUF;			// increment de l'index
			_nbmess --;							// maj du nbmess
			pthread_cond_signal(&_full_buff);				// Envoi d'un signal pour dire que le buffer n'est pas plein
			pthread_mutex_unlock(&_mut);					// unlock du mutex
			sleep(1);
		}
	}
}

void * Supervision(void * arg)
{	
int boucle;
      do
      { 
          int reponse;
          boucle=1;
          fflush(stdin);
          scanf("%d",&reponse);
          switch(reponse){
                      case 1 :_arret_lecture=1;printf("********************\n\n\tPausse affichage\n\n********************\n");break;
                      case 2 :printf("********************\n\n\tReprise\n\n********************\n");_arret_lecture=0;break;
                      case 0 :	_arret_tot = 1;
				printf("Fin tâche Supervision\n");
				pthread_cond_signal(&_empty_buff);			// reveil des tâches
				pthread_cond_signal(&_full_buff);			
				pthread_exit(0);
                      default :break;
                      }
          reponse=0;
	  sleep(1);
      }while(boucle !=0);
}


int main(void)
{
	pthread_t threadEcriture, threadLecture, threadSupervision;
	if(pthread_create(&threadEcriture,NULL,Ecriture,NULL)!=0)			// creation thread entrée
	{
		perror("creation thread entrée");
		exit(1);
	}

	if(pthread_create(&threadLecture,NULL,Lecture,NULL)!=0)				// creation thread sortir
	{
		perror("creation thread entrée");
		exit(1);
	}
	
	if(pthread_create(&threadSupervision,NULL,Supervision,NULL)!=0)			// creation thread entrée
	{
		perror("creation thread entrée");
		exit(1);
	}

	pthread_join(threadEcriture,NULL);
	pthread_join(threadLecture,NULL);
	pthread_join(threadSupervision,NULL);
	printf("Fin Main\n");
	return 0;
}
