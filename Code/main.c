#include <stdio.h>
#include <stdlib.h>
#include "api_com.h"
#include <unistd.h>
#include <pthread.h>

#define pas 10000

//#define PICALC
//#define TESTMAXABO
//#define TESTSENDIFNOABO
//#define TESTSENDTOIDUKNOW
//#define ABOBOB
//#define ABOCHARLIE
//#define ABODINGO

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

pthread_mutex_t mut_print = PTHREAD_MUTEX_INITIALIZER;

void * Alice(void * arg)
{
    char dec[]="";
    int ret;
    ret = initMsg();
pthread_mutex_lock(&mut_print);
printf("%s  init: %s\n",dec,corresp_errors[ret]);
pthread_mutex_unlock(&mut_print);

communication com;
#ifdef TESTSENDIFNOABO
int a = 2;
ret = sendMsg(&com,2,&a, sizeof(a));
pthread_mutex_lock(&mut_print);
printf("%s  send: %s\n",dec,corresp_errors[ret]);
pthread_mutex_unlock(&mut_print);
#endif // TESTSENDIFNOABO


ret = aboMsg(&com,1);
pthread_mutex_lock(&mut_print);
printf("%s  abo: %s\n",dec,corresp_errors[ret]);
pthread_mutex_unlock(&mut_print);


#ifdef TESTSENDTOIDUKNOW
ret = sendMsg(&com,1000,&a, sizeof(a));
pthread_mutex_lock(&mut_print);
printf("%s  send: %s\n",dec,corresp_errors[ret]);
pthread_mutex_unlock(&mut_print);
#endif // TESTSENDTOIDUKNOW


#ifdef TESTMAXABO
    int i =0;
    communication com[20];
    for(i=0;i<20;i++)
    {
        ret = aboMsg(&com[i],i);
        pthread_mutex_lock(&mut_print);
        printf("%s  abo: %s\n",dec,corresp_errors[ret]);
        pthread_mutex_unlock(&mut_print);
    }
#endif // TESTMAXABO



#ifdef PICALC
    int it = 0;
    int a = 0;
    double pi = 0;

    a = it * pas;
    it++;

    do{
    ret = sendMsg(&com,2,&a, sizeof(a));
    }while(ret != 0);

    a = it * pas;
    it++;
    do{
    ret = sendMsg(&com,3,&a, sizeof(a));
    }while(ret != 0);

    a = it * pas;
    it++;
    do{
    ret = sendMsg(&com,4,&a, sizeof(a));
    }while(ret != 0);

    while(a < 1000000)
    {
        int dest = 0;
        message * mymess;
        ret = recvMsgBlock(&com,&mymess);
        if(ret==SUCCESS)
        {   double * somme = 0;
            if(mymess != NULL)
            {
                somme = mymess->contenu;
                if (somme != NULL)
                {
                pi += *somme;
                dest = mymess->expid;
                free(mymess->contenu);
                free(mymess);
                }

            }
            mymess=NULL;
        }
        pthread_mutex_lock(&mut_print);
        //printf("pi = %.10f\n",4*pi);
        pthread_mutex_unlock(&mut_print);
        a = it * pas;
        it++;
        do{
            ret = sendMsg(&com,dest,&a, sizeof(a));
        }while(ret != 0);

    }
    printf("\n\t*********** \nCalcul terminé Pi=%.10f\n\n",4*pi);
    a = -1;
    do{
    ret = sendMsg(&com,2,&a, sizeof(a));
    }while(ret != 0);
    do{
    ret = sendMsg(&com,3,&a, sizeof(a));
    }while(ret != 0);
    do{
    ret = sendMsg(&com,4,&a, sizeof(a));
    }while(ret != 0);
    //usleep(50000);
    do{
    message * mymess;
    ret = recvMsg(&com,&mymess);
    }while(ret != NO_MSG);

#endif // PICALC

    printf("desabo thread Alice\n");
    desaboMsg(&com);
    pthread_exit(0);
}

void * Bob(void * arg)
{
    char dec[]="\t\t";
    int ret;
    ret = initMsg();
    pthread_mutex_lock(&mut_print);
    printf("%s init: %s\n",dec,corresp_errors[ret]);
    pthread_mutex_unlock(&mut_print);

#ifdef ABOBOB
    communication com;
    ret = aboMsg(&com,2);
    pthread_mutex_lock(&mut_print);
    printf("%s abo: %s\n",dec,corresp_errors[ret]);
    pthread_mutex_unlock(&mut_print);
#endif // ABOBOB
#ifdef PICALC
    int i = 1;
    int j = 0;
    int mod = 0;
    double somme = 0;
    double val = 0;
    double den = 0;
    while( i != -1)
    {
        message * mymess;
        ret = recvMsgBlock(&com,&mymess);
        if(ret==SUCCESS)
        {
            if(mymess != NULL)
                {
                    int* aq = mymess->contenu;
                    if(aq !=NULL)
                    {
                        i = *aq;
                        free(mymess->contenu);
                        free(mymess);
                    }

                }
                mymess=NULL;
        }

        if(i == -1){break;}
        else
        {
        j = 0;
        for(j=i+1; j<= i + pas; j++)
            {
                if (j%2){mod = 1;}
                else{mod = -1;}
                den = 2 * (j-1);
                den ++;
                val = mod * (1/den);
                somme = somme + val;
            }
        }
        do
        {
        ret = sendMsg(&com,1,&somme, sizeof(somme));
        if (ret ==  12){printf("messagerie pleine\n");}
        }while(ret != 0);

        somme = 0;
    }
#endif // PICALC
#ifdef ABOBOB
    printf("desabo thread Bob\n");
    desaboMsg(&com);
    pthread_exit(0);
#endif // ABOBOB
}

void * Charlie(void * arg)
{
    char dec[]="\t\t\t\t";
    int ret;
    ret = initMsg();
    pthread_mutex_lock(&mut_print);
    printf("%s init: %s\n",dec,corresp_errors[ret]);
    pthread_mutex_unlock(&mut_print);

#ifdef ABOCHARLIE
    communication com;
    ret = aboMsg(&com,3);
    pthread_mutex_lock(&mut_print);
    printf("%s abo: %s\n",dec,corresp_errors[ret]);
    pthread_mutex_unlock(&mut_print);
#endif // ABOCHARLIE

#ifdef PICALC
int i = 1;
    int j = 0;
    int mod = 0;
    double somme = 0;
    double val = 0;
    double den = 0;
    while( i != -1)
    {
        message * mymess;
        ret = recvMsgBlock(&com,&mymess);
        if(ret==SUCCESS)
        {
            if(mymess != NULL)
                {
                    int* aq = mymess->contenu;
                    if(aq !=NULL)
                    {
                        i = *aq;
                        free(mymess->contenu);
                        free(mymess);
                    }
                }
                mymess=NULL;
        }

        if(i == -1){break;}
        else
        {
        j = 0;
        for(j=i+1; j<= i + pas; j++)
            {
                if (j%2){mod = 1;}
                else{mod = -1;}
                den = 2 * (j-1);
                den ++;
                val = mod * (1/den);
                somme = somme + val;
            }
        }
        ret = 8;
        do{
        ret = sendMsg(&com,1,&somme, sizeof(somme));
        if (ret ==  12){printf("messagerie pleine\n");}
        }while(ret != 0);
        somme = 0;
    }
#endif // PICALC
#ifdef ABOCHARLIE
    printf("desabo thread Chalie\n");;
    desaboMsg(&com);
#endif // ABOCHARLIE
    pthread_exit(0);
}

void * Dingo(void * arg)
{
    char dec[]="\t\t\t\t\t\t";
    int ret;
    ret = initMsg();
    pthread_mutex_lock(&mut_print);
    printf("%s init: %s\n",dec,corresp_errors[ret]);
    pthread_mutex_unlock(&mut_print);

#ifdef ABODINGO
    communication com;
    ret = aboMsg(&com,4);
    pthread_mutex_lock(&mut_print);
    printf("%s abo: %s\n",dec,corresp_errors[ret]);
    pthread_mutex_unlock(&mut_print);
#endif // ABODINGO
#ifdef PICALC
int i = 1;
    int j = 0;
    int mod = 0;
    double somme = 0;
    double val = 0;
    double den = 0;
    while( i != -1)
    {
        message * mymess;
        ret = recvMsgBlock(&com,&mymess);
        if(ret==SUCCESS)
        {
            if(mymess != NULL)
                {
                    int* aq = mymess->contenu;
                    if(aq !=NULL)
                    {
                        i = *aq;
                        free(mymess->contenu);
                        free(mymess);
                    }

                }
                mymess=NULL;
        }

        if(i == -1){break;}
        else
        {
        j = 0;
        for(j=i+1; j<= i + pas; j++)
            {
                if (j%2){mod = 1;}
                else{mod = -1;}
                den = 2 * (j-1);
                den ++;
                val = mod * (1/den);
                somme = somme + val;
            }
        }
        do{
        ret = sendMsg(&com,1,&somme, sizeof(somme));
        if (ret ==  12){printf("messagerie pleine\n");}
        }while(ret != 0);
        somme = 0;
    }

#endif // PICALC
#ifdef ABODINGO
    printf("desabo thread Dingo\n");;
    desaboMsg(&com);
#endif // ABODINGO
    pthread_exit(0);
}


int main(void)
{
	pthread_t thread_Alice, thread_Bob, thread_Charlie, thread_Dingo;
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

    if(pthread_create(&thread_Dingo,NULL,Dingo,NULL)!=0)			// creation thread entrée
	{
		perror("creation thread Dingo\n");
		exit(1);
	}
	pthread_join(thread_Alice,NULL);
	pthread_join(thread_Bob,NULL);
	pthread_join(thread_Charlie,NULL);
	pthread_join(thread_Dingo,NULL);
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
    if(ret==SUCCES)
    {
        int* aq = mymess->contenu;
        printf("message: %d     recu a %ld\n",*aq,mymess->tv.tv_sec);
        free(mymess->contenu);
        free(mymess);
		mymess=NULL;
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
    if(ret==SUCCES)
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
