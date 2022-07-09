#include "../common/ft232.h"
#include <stdio.h>

int main(void)
{
    Ft232 ft232;
    if (!ft232.Initialize(9600))
    {
        printf("init error\n");
        exit(1);
    }

    ft232.SetTimeout(30000);
    printf("Ft232 initialized\n");
    char buf[256];
    int nrecv;
    char c = 0;
    while (1)
    {
        //int rc = ft232.Read(buf, nrecv, 100000);
        //printf("rc=%d, nrecv: %d, %d\n", rc, nrecv, buf[0]);
        
        //
        buf[0] = c;
        ft232.Send(buf, 1);
        ++c;
        Sleep(2000);
        //
    }
}