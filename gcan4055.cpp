#include "gcan4055.h"

GCAN4055::GCAN4055(int n)
{
    status=pre_operational;
    ID=n;
    int i=0;
    for(i=0;i<8;i++)
    {
        DI[i]=0;
        DO[i]=0;
    }

}
