#include <stdio.h>
#include <stdlib.h>


int main()
{
    char choix;
    do
    {    system("clear");

         choix=toupper(getch());
	 printf("\n\n%c\n\n",choix);
    }
    while(choix!='Q');
    return 0;
}
