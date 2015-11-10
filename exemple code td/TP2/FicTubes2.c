// DZIECIOL NICOLAS

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>  /* pour fork() */
#include <stdlib.h>  /* pour exit() */
#include <sys/wait.h> /* pour wait */
#include <signal.h>  /* pour sleep */
#define TBUFFER 20

int fils1(int tube[], char* argv)
{
	close(tube[0]);
	int fd,NbRead;
	char buf[TBUFFER];
	
	if ((fd=open (argv,O_RDONLY))==-1)		// ouverture du fichier ou sera ecrit le texte à convertir
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
{
	close(tube[1]);

	close(0);
	dup(tube[0]);
	close(tube[0]);
	
	execlp("./minmaj",NULL);
	perror("minmaj");
	return 0;
}


int main(int argc, char** argv)
{
	int pid1=0, pid2=0;
	int tube[2];
	int s;
	
	if(argc<2){
	perror("erreur");
	exit(1);
	}


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
		fils1(tube, argv[1]);	// appel de la fonction du fils 1
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


