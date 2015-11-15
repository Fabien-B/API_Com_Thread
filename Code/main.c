#include <stdio.h>
#include <stdlib.h>
#include "api_com.h"
#include <unistd.h>
#include <pthread.h>

int main()
{
    int ret;
    printf("Hello world!\n");

    ret = initMsg();
    printf("Valeur de retour init: %d\n",ret);


    communication com1;
    ret = aboMsg(&com1,5);
    printf("abo %d: %d\n",com1.client_id,ret);

    communication com2;
    ret = aboMsg(&com2,51);
    printf("abo %d: %d\n",com2.client_id,ret);

    communication com3;
    ret = aboMsg(&com3,15);
    printf("abo %d: %d\n",com3.client_id,ret);

    //envoie d'un int
    int a = 42;
    ret = sendMsg(&com1,15,&a, sizeof(a));
    printf("send : %d\n", ret);

    //envoie d'une chaine de caractères
    char b[50] = "Si tu vois ce message, c'est gagne !";
    ret = sendMsg(&com2,15,&b, 50*sizeof(char));
    printf("send : %d\n", ret);

    //reception d'un int
    message * mymess;
    ret = recvMsg(&com3,&mymess);
    printf("recv : %d\n", ret);
    int* aq = mymess->contenu;
    printf("message: %d\n",*aq);

    //reception d'une chaine de caractères
    message * mymess2;
    ret = recvMsg(&com3,&mymess2);
    printf("recv : %d\n", ret);
    char * chaine = mymess2->contenu;
    printf("message: %s\n",chaine);

    ret = finMsg();
    printf("Valeur de retour fin: %d\n",ret);
    return 0;
}
