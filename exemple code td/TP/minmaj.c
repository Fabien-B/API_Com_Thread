#include <stdio.h>
#include <unistd.h>
#define TBUFFER 20 // Taille du buffer

int main(int argc, char ** argv)
{		
	char buffer[TBUFFER]; // Definition de la chaine de caractère dans laquelle sera effectuer les modifocations des caractère
	int i;		// Entier de boucle
	int NbRead;	// Entier dans lequel sera mis le nombre de caractère présent dans le buffer
	int NbWrite;	// Entier dans lequel sera mis le nombre de caractère écris à l'écran

	while((NbRead = read(0,buffer,TBUFFER))> 0)	// Boucle avec pour condition d'arret le nombre de caractère présent dans le buffer
	{
		for(i=0;i<NbRead;i++)			// Boucle qui met les minuscules en majuscules
		{
			if(buffer[i]>='a' && buffer[i]<='z')
			{
				buffer[i] += 'A' - 'a';
			}
		}
	
		NbWrite = write(1,buffer,NbRead);	// ecrit le buffer à l'écran
	
		if(NbWrite != NbRead)	// test d'erreur si le nombre de caractère ecris est different du nombre de caractère lu
		{
			perror("Erreur");	
		}
	}
	return 0;
}
