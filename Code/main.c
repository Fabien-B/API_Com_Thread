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
//usleep(1000);
    communication moi;
    moi.client_id = 2;
    ret = aboMsg(&moi);
    printf("Valeur de retour abo: %d\n",ret);


    communication moi2;
    moi2.client_id = 4;
    ret = aboMsg(&moi2);
//usleep(1000);
    printf("Valeur de retour abo: %d\n",ret);

    communication moi3;
    moi3.client_id = 6;
    ret = aboMsg(&moi3);
//usleep(1000);
    printf("Valeur de retour abo: %d\n",ret);

    int a = 621;
    ret = sendMsg(&moi3, 4, &a);
    printf("Valeur de retour sendd %d\n",ret);
sleep(2);
    ret = finMsg();
    printf("Valeur de retour fin: %d\n",ret);
    sleep(2);
    return 0;
}
