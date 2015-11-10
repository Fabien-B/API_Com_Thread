// DZIECIOL Nicolas tp3

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>




int main(int argc, char ** argv)
{
	typedef struct			/// definition structure infoAppli
	{
		pid_t pid;
		char message[80];
		float valeur;
	}infoAppli;

	if (argc != 2)	/// test des arguments entrés lors du lancement  || ((strcmp(argv[1],"info")) != 0 || (strcmp(argv[1],"fin"))!= 0 )
	{
		perror("Mauvais argument");
		exit(1);
	}
	
	int cle, id;
	infoAppli * adresse;
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
	

	if (strcmp(argv[1],"info")==0)		/// partie info 
	{
		printf("Le processus avec le pid %d, envoi le message suivant %s et la valeur %f\n",adresse->pid, adresse->message,adresse->valeur);
		shmdt(adresse);			/// détachement de la zone
	}

	if (strcmp(argv[1],"fin")==0)		/// partie fin
	{
		printf("fin du processus %d, suppression de la zone memoire partagée",pidapp);	
		kill((*adresse).pid,2);		/// suppression du processus
		shmdt(adresse);
		shmctl(id, IPC_RMID,NULL);
	}

	
	
	return 0;

	

	
	
}



