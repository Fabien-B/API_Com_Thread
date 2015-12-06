#include <stdio.h>
#include <stdlib.h>
#include "api_com.h"
#include <unistd.h>
#include <pthread.h>

char* corresp_errors[] = {"SUCCESS",
"ALREADY_LAUNCH",
"INIT_ERROR",
"NO_SERVICE",
"ID_IN_USE",
"MAX_ABO",
"NO_ABO",
"ID_UNKNOWN",
"NO_MSG",
"SERVICE_USED",
"TECH_ERROR",
"STILL_REMAINS_ABOS",
"INBOX_FULL"};


void * Alice(void * arg)
{
    char dec[]="";
    int ret;
    ret = initMsg();
    printf("%s init: %s\n",dec,corresp_errors[ret]);
    //int res;
    //isAbo(5,&res);
    communication com;
    ret = aboMsg(&com,5);
    printf("%s abo: %s\n",dec,corresp_errors[ret]);

    pthread_exit(0);
}

void * Bob(void * arg)
{
    char dec[]="\t\t";
    int ret;
    ret = initMsg();
    printf("%s init: %s\n",dec,corresp_errors[ret]);

    int res;
    isAbo(5,&res);
    /*communication com;
    ret = aboMsg(&com,15);
    printf("%s abo: %s\n",dec,corresp_errors[ret]);*/


    pthread_exit(0);
}

void * Charlie(void * arg)
{
    char dec[]="\t\t\t\t";
    int ret;
    ret = initMsg();
    printf("%s init: %s\n",dec,corresp_errors[ret]);

    int res;
    isAbo(5,&res);
    /*communication com;
    ret = aboMsg(&com,5);
    printf("%s abo: %s\n",dec,corresp_errors[ret]);*/


    pthread_exit(0);
}


int main(void)
{
	pthread_t thread_Alice, thread_Bob, thread_Charlie;
	if(pthread_create(&thread_Alice,NULL,Alice,NULL)!=0)			// creation thread entrée
	{
		perror("creation thread Alice\n");
		exit(1);
	}

	if(pthread_create(&thread_Bob,NULL,Bob,NULL)!=0)				// creation thread sortir
	{
		perror("creation thread Bob\n");
		exit(1);
	}

	if(pthread_create(&thread_Charlie,NULL,Charlie,NULL)!=0)			// creation thread entrée
	{
		perror("creation thread Charlie\n");
		exit(1);
	}

	pthread_join(thread_Alice,NULL);
	pthread_join(thread_Bob,NULL);
	pthread_join(thread_Charlie,NULL);
	printf("Fin Main\n");
	return 0;
}






/*
int main()
{
    int ret;
    printf("Hello world!\n");

    ret = initMsg();
    printf("Valeur de retour init: %d\n",ret);


    communication com1;
    ret = aboMsg(&com1,5);
    //printf("abo %d: %d\n",com1.client_id,ret);

    communication com2;
    ret = aboMsg(&com2,5);
    //printf("abo %d: %d\n",com2.client_id,ret);

    communication com3;
    ret = aboMsg(&com3,15);
    //printf("abo %d: %d\n",com3.client_id,ret);

    //envoie d'un int
    int a = 42;
    ret = sendMsg(&com1,15,&a, sizeof(a));
    printf("send : %d\n", ret);

    //envoie d'une chaine de caractères
    char b[50] = "Si tu vois ce message, c'est gagne !";
    int i;
    for(i=0;i<10;i++)
    {
		ret = sendMsg(&com3,15,&b, 50*sizeof(char));
	}

    ret = getNbAbo(&a);
    printf("get nb abo : %d      nbAbo: %d\n", ret,a);

    ret = isAbo(15,&a);
    printf("is abo 15: %d      res: %d\n", ret,a);

    ret = isAbo(31,&a);
    printf("is abo 31: %d      res: %d\n", ret,a);

    //reception d'un int
    message * mymess;
    ret = recvMsg(&com3,&mymess);
    printf("recv : %d\n", ret);
    if(ret==0)
    {
        int* aq = mymess->contenu;
        printf("message: %d     recu a %ld\n",*aq,mymess->tv.tv_sec);
        free(mymess->contenu);
        free(mymess);
    }

	//reception du nombre de message dans notre messagerie
	int nb_msg;
    ret = getNbMsg(&com3,&nb_msg);
    printf("recv nb_msg_mess: %d\n", ret);
    printf("Nombre de messages: %d\n",nb_msg);

    //reception d'une chaine de caractères
    message * mymess2 = NULL;
    ret = recvMsg(&com3,&mymess2);
    printf("recv : %d\n", ret);
    if(ret==0)
    {
        char * chaine = mymess2->contenu;
        printf("message: %s    reçu à %ld\n",chaine,mymess2->tv.tv_sec);
        free(mymess2->contenu);
        free(mymess2);
    }

    ret = desaboMsg(&com1);
    printf("retour Desabo: %d\n",ret);

    ret = aboMsg(&com1,5);
    printf("abo %d: %d\n",com1.client_id,ret);

    ret = finMsg();
    printf("Valeur de retour fin: %d\n",ret);
    return 0;
}
*/
