#include "crc.h"

void CRC_compute(uint8_t * data, uint32_t len)
{
    uint32_t i;
    uint8_t crc=data[0];
    uint8_t crc2=0;

    for (i=1;i<(len+1);i++)
    {
        crc2=data[i] ^ Table87[crc];
        crc=crc2;
    }
    data[len+1]=crc;

    crc=data[0];
    for (i=1;i<(len+1);i++)
    {
        crc2=data[i] ^ Table95[crc];
        crc=crc2;
    }
    data[len+2]=crc;
}

uint8_t CRC_check(uint8_t * data, uint32_t len)
{
    //—читаем первый байт CRC
    uint8_t crc=data[0];
    uint8_t crc2=0;
    uint32_t i;
    for (i=1;i<(len+1);i++)
    {
        crc2=data[i] ^ Table87[crc];
        crc=crc2;
    }
    uint8_t byte1=crc;

    //—читаем второй байт CRC
    crc=data[0];
    for (i=1;i<(len+1);i++)
    {
        crc2=data[i] ^ Table95[crc];
        crc=crc2;
    }
    uint8_t byte2=crc;
    //qDebug() <<(quint8)recData[(*len)-2] <<" " <<byte1 <<" " <<(quint8)recData[(*len)-1] <<" " <<byte2;
    //провер€ем CRC
    if (((uint8_t)data[len+1]!=byte1) ||
        ((uint8_t)data[len+2]!=byte2))
        return 0;
    else
        return 1;
}