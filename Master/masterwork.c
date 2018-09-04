#include "masterwork.h"
#include "transceiver.h"

uint8_t slave_id[30];
uint8_t slave_num;
uint32_t slave_cal_temp[30][40];
uint32_t slave_temp[30][30];
uint8_t slave_temp_num[30];
uint8_t slave_com[30][30];

void MasterWork(uint8_t * buffer)
{
  uint8_t i=0;
  uint8_t j;
  uint8_t cur_slave_id;
  
  cur_slave_id=buffer[1];
  if(buffer[3]==0x24)
  {
    while((slave_id[i]!=cur_slave_id)&&(i<slave_num)) i++;
    if (i==slave_num) { slave_id[i]=cur_slave_id; slave_num++; }
    buffer[2]=cur_slave_id;
    buffer[3]=0x21;
    transceiverSendPacket(buffer, 3);
    transceiverRXOn();
    
    if(transceiverReadPacket(buffer))
    {  
      for(j=4; j<buffer[0]; j+=4) 
      slave_temp[i][(j-4)/4]=(buffer[j]<<24)|(buffer[j+1]<<16)|(buffer[j+2]<<8)|(buffer[j+3]);
      slave_temp_num[i]=(buffer[0]-3)/4;  
    }
    else
    {
      slave_temp[i][0]=0x65720021;
      slave_temp_num[i]=1;
    }
  } else
  if(buffer[3]==0x22)
  {
    while((slave_id[i]!=cur_slave_id)&&(i<slave_num)) i++;
    if (i==slave_num) { slave_id[i]=cur_slave_id; slave_num++; }
    buffer[2]=cur_slave_id;
    buffer[3]=0x22;
    transceiverSendPacket(buffer, 3);
    transceiverRXOn();
    
    if(transceiverReadPacket(buffer))
    {  
      for(j=4; j<buffer[0]; j+=4) 
      slave_cal_temp[i][(j-4)/4]=(buffer[j]<<24)|(buffer[j+1]<<16)|(buffer[j+2]<<8)|(buffer[j+3]);  
    }
    else
    {
      slave_cal_temp[i][0]=0x65720022;
    }
    
    buffer[2]=cur_slave_id;
    buffer[3]=0x23;
    transceiverSendPacket(buffer, 3);
    transceiverRXOn();
    
    if(transceiverReadPacket(buffer))
    {  
      for(j=4; j<buffer[0]-4; j+=4) 
      slave_cal_temp[i][16+(j-4)/4]=(buffer[j+4]<<24)|(buffer[j+5]<<16)|(buffer[j+6]<<8)|(buffer[j+7]);  
    }
    else
    {
      slave_cal_temp[i][16]=0x65720023;
    }
  }
  
  if(slave_com[i][0]!=0)
  {
      buffer[2]=cur_slave_id;
      for(j=0; j<slave_com[i][0]; j++)
      {
        buffer[j+3]=slave_com[i][j+1];
      }
      transceiverSendPacket(buffer, 2+slave_com[i][0]);
      slave_com[i][0]=0;
      transceiverRXOn();
      transceiverReadPacket(buffer);
      if(slave_com[i][1]==0x13) return;
  }    
  buffer[2]=slave_id[i];
  buffer[3]=0xF1;
  transceiverSendPacket(buffer, 3);
  transceiverRXOn();
  transceiverReadPacket(buffer);
}