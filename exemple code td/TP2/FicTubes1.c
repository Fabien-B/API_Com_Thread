// DZIECIOL NICOLAS

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>  /* pour fork() */
#include <stdlib.h>  /* pour exit() */
#include <sys/wait.h> /* pour wait */
#include <signal.h>  /* pour sleep */
#define TBUFFER 20

int fils1(int tube[])
{
	close(tube[0]);
	int fd,NbRead;
	char buf[TBUFFER];
	
	if ((fd=open ("txt",O_RDONLY))==-1)		// ouverture du fichier ou sera ecrit le texte à convertir
	{
		perror("Ouverture");
		exit(1);
	}

	while((NbRead=read(fd, buf, sizeof(buf)))>0)
	{
		write(tube[1],buf,NbRead);		// ecriture sur le tube
	}
	close(fd);
	close(tube[1]);
	return 0;
}

int fils2(int tube[])
{		// Fonction minmaj
	close(tube[1]);
	printf("\nResultat : \n");

	char buffer[TBUFFER];
	int i;
	int NbRead;

	while((NbRead = read(tube[0],buffer,TBUFFER))> 0)		// lecture à l'entrée du tube
	{
		for(i=0;i<NbRead;i++)
		{
			if(buffer[i]>='a' && buffer[i]<='z')
			{
				buffer[i] += 'A' - 'a';
			}
		}

		int NbWrite = write(1,buffer,NbRead);
		if(NbWrite != NbRead)
		{
			perror("Erreur");
		}
	}
	close(tube[0]);
	return 0;
}


int main(void)
{
	int pid1=0, pid2=0;
	int tube[2];
	int s;



	if (pipe(tube) == -1) 	// Je creer le tube de communication avant le fork
	{
		perror("Creation pipe impossible");
		exit(1);
	}


	
	if ((pid1=fork()) == -1)		// creation du fils 1
	{
		perror("Erreur creation fils 1");
		exit(1);
	}

	if (pid1 == 0) 
	{
	    /* Code du fils 1 */
		fils1(tube);	// appel de la fonction du fils 1
		exit(0);
	}

	  /* Code du pere */

	if ((pid2=fork()) == -1)		// creation du fils 2
	{
		perror("Erreur creation fils 2");
		exit(1);
	}

	if (pid2 == 0) 
	{
	    /* Code du fils 2 */
		fils2(tube); 	// appel de la fonction du fils 2
		exit(0);
	}

	/* Code du pere */
	close(tube[0]);	// fermeture de l'accès au tube par le pere
	close(tube[1]);
	wait(&s);
	wait(&s);

	return(0);
}


