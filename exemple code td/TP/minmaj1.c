#include <stdio.h>
#include <unistd.h>
#define TBUFFER 20

int main(int argc, char ** argv)
{		
	char buffer[TBUFFER];
	int i;
	int NbRead;

	while((NbRead = read(0,buffer,TBUFFER))> 0)
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
	return 0;
}
