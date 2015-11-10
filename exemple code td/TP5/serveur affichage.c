// DZIECIOL Nicolas tp5 serveur d'affichage

#define _TAILLEBUF 50
#define _TAILLEMESS 30
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>

char _buffer[_TAILLEBUF][_TAILLEMESS];
int _nbmess = 0;
pthread_cond_t _full_buff = PTHREAD_COND_INITIALIZER;
pthread_cond_t _empty_buff = PTHREAD_COND_INITIALIZER;
pthread_mutex_t _mut = PTHREAD_MUTEX_INITIALIZER;


void * Ecriture(void * arg)
{
	int index_ecriture = 0;	
	while(1)
	{
		pthread_mutex_lock(&_mut);
		while(_nbmess == _TAILLEBUF)
		{
			pthread_cond_wait(&_full_buff, &_mut);
		}
		sprintf(_buffer[index_ecriture],"il y a %d voitures",index_ecriture);
		_nbmess ++;
		index_ecriture = (index_ecriture+1)%_TAILLEBUF;
		pthread_cond_signal(&_empty_buff);
		pthread_mutex_unlock(&_mut);
		sleep(1);
	}
}

void * Lecture(void * arg)
{	
	int index_lecture = 0;
	while(1)
	{
		pthread_mutex_lock(&_mut);
		while(_nbmess == 0)
		{
			pthread_cond_wait(&_empty_buff, &_mut);
		}
		printf("%s\n",_buffer[index_lecture]);
		index_lecture = (index_lecture+1)%_TAILLEBUF;
		_nbmess --;		
		pthread_cond_signal(&_full_buff);
		pthread_mutex_unlock(&_mut);
		sleep(1);
	}
}


int main(void)
{
	pthread_t threadEcriture, threadLecture;
	if(pthread_create(&threadEcriture,NULL,Ecriture,NULL)!=0)	// creation thread entrée
	{
		perror("creation thread entrée");
		exit(1);
	}

	if(pthread_create(&threadLecture,NULL,Lecture,NULL)!=0)	// creation thread sortir
	{
		perror("creation thread entrée");
		exit(1);
	}

	pthread_join(threadEcriture,NULL);
	pthread_join(threadLecture,NULL);
	printf("fin main\n");
	return 0;
}
