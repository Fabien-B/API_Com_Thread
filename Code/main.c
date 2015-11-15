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

    ret = finMsg();
    printf("Valeur de retour fin: %d\n",ret);
    return 0;
}
