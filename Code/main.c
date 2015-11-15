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
    printf("ttttttt: %d\n",ret);

    communication com2;
    ret = aboMsg(&com2,6);
    printf("------: %d\n",ret);


    int a = 123;
    sendMsg(&com1,5,&a);

    ret = finMsg();
    printf("Valeur de retour fin: %d\n",ret);
    return 0;
}
