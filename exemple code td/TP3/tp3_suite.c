// DZIECIOL Nicolas tp3 suite

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <limits.h>




typedef struct			/// definition structure infoAppli
	{
		pid_t pid;
		char message[80];
		float valeur;
	}infoAppli;

infoAppli * adresse;
int id;




void espion(int numSig) 
{
	switch(numSig)		/// application de info ou fin en fonction du signal reçu
	{
		case 16:
		{
			printf("Le processus avec le pid %d, envoi le message suivant %s et la valeur %f\n",adresse->pid, adresse->message,adresse->valeur);
		signal(SIGUSR1, espion); /* On rearme le signal */
			break;
		}
		case 17:
		{
			kill((*adresse).pid,2);		/// suppression du processus
			shmdt(adresse);
			shmctl(id, IPC_RMID,NULL);
		exit(0);
		}
	}
}




int main(int argc, char ** argv)
{	
	printf("\n\nPID : %d\n\n",getpid());
	/* on interdit au processus de recevoir CTRL-C (SIGINT). */
  	signal(SIGINT, SIG_IGN);

	
	/* Installation du handler espion() pour SIGUSR1 et SIGUSR2. */

	signal(16, espion);
	signal(17, espion);
	
	int cle;
	int i;
	pid_t pidapp;
	char msg[80];
	float valeur;

	cle = ftok("./applicationTD3",1);	/// genération de la clé
	if(cle==-1)	/// test si elle a bien été intégrée
	{
		perror("clé");
		exit(1);
	}	


	int taille = sizeof(infoAppli);

	id = shmget(cle, taille, 0);		/// ouverture de la zone memoire partagée
	if ( id == -1)
	{
		perror("Ouverture memoire");
		exit(1);
	}

	adresse = shmat(id, NULL,SHM_RDONLY);	/// attachement de la zone memoire
	if(adresse==(void*)-1)
	{
		perror("Attachement memoire partagé");
		shmctl(id, IPC_RMID,NULL);		/// suppression du segment
		exit(1);
	}
	
	while(adresse->valeur<LONG_MAX)
	{
		sleep(1);
	}
	
	return 0;
}



