#ifndef GCAN4055_H
#define GCAN4055_H
#define nodeStateIDPre 0x700
#define NMTIDPre 0x00
#define RPDOIDPre 0x200
#define TPDOIDPre 0x180
class GCAN4055
{
public:
    GCAN4055(int n);
public:
    bool DI[8];//
    bool DO[8];//
    int ID;
    enum status_type
    {
        pre_operational,operational,stopped,not_found
    }status;

};

#endif // GCAN4055_H
