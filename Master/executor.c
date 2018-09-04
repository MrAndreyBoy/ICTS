#include "executor.h"
#include "stm32l1xx_it.h"
#include "tsensor.h"
#include "onewire.h"
#include "uartwire.h"
#include "transceiver.h"
#include "cc112x_spi.h"
#include "rtc.h"
#include "memory.h"
#include "masterwork.h"
#include "crc.h"

void sendPacket(uint8_t buffer[], uint8_t len)
{
  uint8_t i;
  
  buffer[0]=len;
  buffer[1]=getMemory(ADDR_MASTER_ID);
  //CRC_compute(buffer, len);
  
  for(i=0; i<len+1/*3*/; i++)
  {
    USART_SendData(USART3, (buffer[i]));
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
    {}
  }
}

void executor()
{
    uint8_t *Data;
    uint16_t temp;
    uint32_t temp32;
    uint8_t buf[256]={0};
    uint8_t i;
        
    if(packet_ready)
    {
      /*if(readbuffer[1]!=getMemory(ADDR_MASTER_ID))
      {
        packetlength=0;
        packet_ready=0;
        return;
      }
      if(!CRC_check(readbuffer, readbuffer[0]))
      {
        buf[2]=Data[0];
        buf[3]='e';
        buf[4]='s';
        sendPacket(buf, 4);
        packetlength=0;
        packet_ready=0;
        return;
      }*/
      
      Data=readbuffer+2;
      if(Data[0]==0x01) //set SLAVE_ID
      {
        buf[2]=Data[1];
        buf[3]=0x01;
        buf[4]=Data[2];
        transceiverSendPacket(buf, 4);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]=buf[4];
          buf[4]=buf[5];
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x02) //set MASTER_ID
      {
        buf[2]=Data[1];
        buf[3]=0x02;
        buf[4]=Data[2];
        transceiverSendPacket(buf, 4);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]=buf[4];
          buf[4]=buf[5];
          sendPacket(buf,4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf,4);
        }
      } else
      if(Data[0]==0x03) //set Slave Mode
      {
        buf[2]=Data[1];
        buf[3]=0x03;
        buf[4]=Data[2];
        transceiverSendPacket(buf, 4);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x04) //set Slave Current Time
      {
        buf[2]=Data[1];
        buf[3]=0x04;
        buf[4]=Data[2];
        buf[5]=Data[3];
        buf[6]=Data[4];
        buf[7]=Data[5];
        transceiverSendPacket(buf, 7);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x05) //set Slave nearest Time of Measure
      {
        buf[2]=Data[1];
        buf[3]=0x05;
        buf[4]=Data[2];
        buf[5]=Data[3];
        buf[6]=Data[4];
        buf[7]=Data[5];
        transceiverSendPacket(buf, 7);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x06) //set Slave Measure Interval
      {
        buf[2]=Data[1];
        buf[3]=0x06;
        buf[4]=Data[2];
        transceiverSendPacket(buf, 4);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x07) //set Slave Measure Count
      {
        buf[2]=Data[1];
        buf[3]=0x07;
        buf[4]=Data[2];
        transceiverSendPacket(buf, 4);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x08) //set Slave nearest Time of Transmit
      {
        buf[2]=Data[1];
        buf[3]=0x08;
        buf[4]=Data[2];
        buf[5]=Data[3];
        buf[6]=Data[4];
        buf[7]=Data[5];
        transceiverSendPacket(buf, 7);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x09) //set start measure number
      {
        buf[2]=Data[1];
        buf[3]=0x09;
        buf[4]=Data[2];
        buf[5]=Data[3];
        transceiverSendPacket(buf, 5);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x10) //set tsensor's number and address
      {
        buf[2]=Data[1];
        buf[3]=0x10;
        buf[4]=Data[2];
        for(i=0; i<buf[4]; i++)
        {
          buf[5+i*8]=Data[3+i*8];
          buf[6+i*8]=Data[4+i*8];
          buf[7+i*8]=Data[5+i*8];
          buf[8+i*8]=Data[6+i*8];
          buf[9+i*8]=Data[7+i*8];
          buf[10+i*8]=Data[8+i*8];
          buf[11+i*8]=Data[9+i*8];
          buf[12+i*8]=Data[10+i*8];
        }
        transceiverSendPacket(buf, 4+buf[4]*8);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x11) //set deferred command for slave
      {
        i=0;
        while((slave_id[i]!=Data[1])&&(i<slave_num)) i++;
        if (i==slave_num) 
        { 
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
        else
        {
          for(int j=0; j<=Data[2]; j++)
          {
            slave_com[i][j]=Data[j+2];
          }
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x12) //set TEST flag
      {
        buf[2]=Data[1];
        buf[3]=0x12;
        buf[4]=Data[2];
        transceiverSendPacket(buf, 4);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x13) //set STOP flag
      {
        buf[2]=Data[1];
        buf[3]=0x13;
        transceiverSendPacket(buf, 3);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x15) //set Slave start calibration time
      {
        buf[2]=Data[1];
        buf[3]=0x15;
        buf[4]=Data[2];
        buf[5]=Data[3];
        buf[6]=Data[4];
        buf[7]=Data[5];
        transceiverSendPacket(buf, 7);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x18) //set Slave nearest Time of Transmit calibration data
      {
        buf[2]=Data[1];
        buf[3]=0x18;
        buf[4]=Data[2];
        buf[5]=Data[3];
        buf[6]=Data[4];
        buf[7]=Data[5];
        transceiverSendPacket(buf, 7);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x19) //set start calibration number
      {
        buf[2]=Data[1];
        buf[3]=0x19;
        buf[4]=Data[2];
        buf[5]=Data[3];
        transceiverSendPacket(buf, 5);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x20) //get tsensor's number and address
      {
        buf[2]=Data[1];
        buf[3]=0x20;
        transceiverSendPacket(buf, 3);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]=buf[4];
          for(i=0; i<buf[3]; i++)
          {
            buf[4+i*8]=buf[5+i*8];
            buf[5+i*8]=buf[6+i*8];
            buf[6+i*8]=buf[7+i*8];
            buf[7+i*8]=buf[8+i*8];
            buf[8+i*8]=buf[9+i*8];
            buf[9+i*8]=buf[10+i*8];
            buf[10+i*8]=buf[11+i*8];
            buf[11+i*8]=buf[12+i*8];
          }
          sendPacket(buf, 3+buf[3]*8);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x21) //get packet of data
      {
        i=0;
        while((slave_id[i]!=Data[1])&&(i<slave_num)) i++;
        if (i==slave_num) 
        { 
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          for(int j=0; j<slave_temp_num[i]; j++)
          {
            buf[4*j+3]=(slave_temp[i][j]>>24)&0xFF;
            buf[4*j+4]=(slave_temp[i][j]>>16)&0xFF;
            buf[4*j+5]=(slave_temp[i][j]>>8)&0xFF;
            buf[4*j+6]=slave_temp[i][j]&0xFF;
          }
          sendPacket(buf,((slave_temp_num[i])*4)+2); 
        }
      } else
      if(Data[0]==0x22) //get packet of calibration data
      {
        i=0;
        while((slave_id[i]!=Data[1])&&(i<slave_num)) i++;
        if (i==slave_num) 
        { 
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          for(int j=0; j</*(3+1)*/(30+1); j++)
          {
            buf[4*j+3]=(slave_cal_temp[i][j]>>24)&0xFF;
            buf[4*j+4]=(slave_cal_temp[i][j]>>16)&0xFF;
            buf[4*j+5]=(slave_cal_temp[i][j]>>8)&0xFF;
            buf[4*j+6]=slave_cal_temp[i][j]&0xFF;
          }
          sendPacket(buf,(/*(3+1)*/(30+1)*4)+2); 
        }
      } else
      if(Data[0]==0x24) //get Slave Current Time
      {
        buf[2]=Data[1];
        buf[3]=0x24;
        transceiverSendPacket(buf, 3);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]=buf[4];
          buf[4]=buf[5];
          buf[5]=buf[6];
          buf[6]=buf[7];
          sendPacket(buf, 6);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }
      } else
      if(Data[0]==0x31) //get Current 1-Wire Temperature
      {
        buf[2]=Data[1];
        buf[3]=0x31;
        transceiverSendPacket(buf, 3);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]=buf[4];
          buf[4]=buf[5];
          buf[5]=buf[6];
          buf[6]=buf[7];
          sendPacket(buf,6);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf,4);
        }
      } else
      if(Data[0]==0x32) //get Calibration Packet from Slave
      {
        buf[2]=Data[1];
        buf[3]=0x32;
        buf[4]=Data[2];
        transceiverSendPacket(buf, 4);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]=Data[2];
          sendPacket(buf,buf[0]);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf,4);
        }
      } else
      if(Data[0]==0x33) //get Calibration Packet from Slave
      {
        buf[2]=Data[1];
        buf[3]=0x33;
        buf[4]=Data[2];
        transceiverSendPacket(buf, 4);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]=Data[2];
          sendPacket(buf,127);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf,4);
        }
      } else
      if(Data[0]==0x41) //get Power
      {
        buf[2]=Data[1];
        buf[3]=0x41;
        transceiverSendPacket(buf, 3);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          temp=transceiverReadReg(CC112X_RSSI1)<<4;
          i=transceiverReadReg(CC112X_RSSI0)>>3;
          temp=temp|(i&0x0F);
          
          buf[2]=Data[0];
          buf[3]=buf[4];
          buf[4]=buf[5];
          buf[5]=(uint8_t)(temp>>8);
          buf[6]=(uint8_t)temp;
          sendPacket(buf,6);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf,4);
        }
      } else
      if(Data[0]==0x42) //set transmit Power
      {
        buf[2]=Data[1];
        buf[3]=0x42;
        buf[4]=Data[2];
        transceiverSendPacket(buf, 4);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf,4);
          transceiverChangeReg(20, (Data[2]+17)*2-1);
          transceiverConfig();
          transceiverManualCalibration();
          transceiverRXOn();
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf,4);
        }
      } else
      if(Data[0]==0x43) //set Speed
      {
        buf[2]=Data[1];
        buf[3]=0x43;
        buf[4]=Data[2];
        transceiverSendPacket(buf, 4);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf,4);
          transceiverChangeReg(10, Data[2]);
          transceiverConfig();
          transceiverManualCalibration();
          transceiverRXOn();
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf,4);
        }
      } else
      if(Data[0]==0x44) //set Modulation
      {
        buf[2]=Data[1];
        buf[3]=0x44;
        buf[4]=Data[2];
        transceiverSendPacket(buf, 4);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf,4);
          if(Data[2]==1) transceiverChangeReg(5, 0x03);
          if(Data[2]==2) transceiverChangeReg(5, 0x0B);
          if(Data[2]==3) transceiverChangeReg(5, 0x23);
          if(Data[2]==4) transceiverChangeReg(5, 0x2B);
          if(Data[2]%2) transceiverChangeReg(4, 0x0B);
          else transceiverChangeReg(4, 0x08);
          transceiverConfig();
          transceiverManualCalibration();
          transceiverRXOn();
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf,4);
        }
      } else
      if(Data[0]==0x45) //set Speed
      {
        buf[2]=Data[1];
        buf[3]=0x45;
        buf[4]=Data[2];
        transceiverSendPacket(buf, 4);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf,4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf,4);
        }
      } else
      if(Data[0]==0x81) //set MASTER_ID to Master 
      {
        setMemory(ADDR_MASTER_ID,Data[1]);
        buf[2]=Data[0];
        buf[3]='o';
        buf[4]='k';  
        sendPacket(buf,4);
      } else
      if(Data[0]==0x83) //set Master Current Time
      {
        setRTChour(Data[1]);
        setRTCmin(Data[2]);
        setRTCsec(Data[3]);
        buf[2]=Data[0];
        buf[3]='o';
        buf[4]='k';  
        sendPacket(buf,4);
      } else
      if(Data[0]==0x93) //get Master Current Time 
      {
        buf[2]=Data[0];
        buf[3]=getRTChour();
        buf[4]=getRTCmin();
        buf[5]=getRTCsec();
        sendPacket(buf,5);
      } else
      if(Data[0]==0xF1) //set Slave to sleep 
      {
        buf[2]=Data[1];
        buf[3]=0xF1;
        transceiverSendPacket(buf, 3);
        transceiverRXOn();
        if(transceiverReadPacket(buf))
        {
          buf[2]=Data[0];
          buf[3]='o';
          buf[4]='k';
          sendPacket(buf, 4);
        }
        else
        {
          buf[2]=Data[0];
          buf[3]='e';
          buf[4]='r';
          sendPacket(buf, 4);
        }        
      } 
      
      else
      if(Data[0]==0x92) //get Current Master Temperature 1-Wire
      {
        temp32=uw_get_term();
        buf[2]=Data[0];
        buf[3]=temp32>>24;
        buf[4]=temp32>>16;
        buf[5]=temp32>>8;
        buf[6]=temp32&0xFF;
        sendPacket(buf, 6);
      } else
      if(Data[0]==0x84) //set Master Current Time
      {
        temp32=((Data[1]<<24)|(Data[2]<<16)|(Data[3]<<8)|(Data[4]));
        setRTCtime(temp32);
        buf[2]=Data[0];
        buf[3]='o';
        buf[4]='k';  
        sendPacket(buf,4);
      } else
      if(Data[0]==0x94) //get Master Current Time
      {
        temp32=getRTCtime();
        buf[2]=Data[0];
        buf[3]=temp32>>24;
        buf[4]=temp32>>16;
        buf[5]=temp32>>8;
        buf[6]=temp32;  
        sendPacket(buf,6);
      }
      
      /*else  
      if(Data[0]==0x07) //set Wake_UP Time to Master 
      {
        uint8_t h,m,s;
        h=Data[1];
        m=Data[2];
        s=Data[3];
        buf[2]=Data[0];
        buf[3]='o';
        buf[4]='k';  
        sendPacket(buf,4);
        gotoSleep(h,m,s);
      } 
      
      
        else
      if(Data[0]==0x52)
      {
        temp=transceiverReadStatus();
        USART_SendData(USART3, (temp&0xFF));
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        {}
      } else
      if(Data[0]==0x53)
      {
        temp=transceiverRead();
        USART_SendData(USART3, (temp&0xFF));
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        {}
      } else
      if(Data[0]==0x54)
      {
        uint8_t buf[100]={0};
        buf[1]=MASTER_ID;
        buf[2]=Data[1];
        transceiverSendPacket(buf, 2);
        transceiverRXOn();
        transceiverReadPacket(buf);
        USART_SendData(USART3, (buf[3]));
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        {}
        USART_SendData(USART3, (buf[4]));
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        {}
      } else
      if(Data[0]==0x55)
      {
        temp=transceiverReadReg(0x2F94);
        USART_SendData(USART3, (temp&0xFF));
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        {}
      } else
      if(Data[0]==0x56)
      {
        temp=transceiverReadReg(0x2F71)<<4;
        temp|=((transceiverReadReg(0x2F70)>>3)&0x0F);
        USART_SendData(USART3, (temp>>8));
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        {}
        USART_SendData(USART3, (temp&0xFF));
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        {}
      }*/
      else
      {
        buf[2]=Data[0];
        buf[3]='e';
        buf[4]='c';
        sendPacket(buf, 4);
      }
      packetlength=0;
      packet_ready=0;
    }
}