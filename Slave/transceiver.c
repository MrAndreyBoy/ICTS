#include "transceiver.h"
#include "cc112x_spi.h"
#include "cc112x_easy_link_reg_config.h"
#include "uartwire.h"
#include "rtc.h"
#include "memory.h"

void transceiverInit()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_40MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_0;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_SetBits(GPIOA, GPIO_Pin_4);
  
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_40MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  GPIO_SetBits(GPIOB, GPIO_Pin_0);
  GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}

void transceiverDeInit()
{
  GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

uint8_t transceiverReset()
{
  uint8_t i;
  uint8_t data=0x30;
  uint8_t recData;
  
  GPIO_ResetBits(GPIOA, GPIO_Pin_4);
  for(i=0;i<8;i++)
  {
    if(data&0x80) GPIO_SetBits(GPIOA, GPIO_Pin_7);
    else GPIO_ResetBits(GPIOA, GPIO_Pin_7);
    recData=recData<<1;
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
    data=data<<1;
    recData|=GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
    GPIO_ResetBits(GPIOA,GPIO_Pin_5);
  }
  while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)){}
  
  GPIO_SetBits(GPIOA, GPIO_Pin_4);
  return recData;
}

uint8_t transceiverSleep()
{
  uint8_t i;
  uint8_t data=0x39;
  uint8_t recData;
  
  GPIO_ResetBits(GPIOA, GPIO_Pin_4);
  for(i=0;i<8;i++)
  {
    if(data&0x80) GPIO_SetBits(GPIOA, GPIO_Pin_7);
    else GPIO_ResetBits(GPIOA, GPIO_Pin_7);
    recData=recData<<1;
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
    data=data<<1;
    recData|=GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
    GPIO_ResetBits(GPIOA,GPIO_Pin_5);
  }
  while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)){}
  
  GPIO_SetBits(GPIOA, GPIO_Pin_4);
  return recData;
}

void transceiverConfig()
{
  uint8_t writeByte;
  uint16_t i;
  
  transceiverReset();
  for(i=0; i<(sizeof(preferredSettings)/sizeof(registerSetting_t)); i++) 
  {
    writeByte = preferredSettings[i].data;
    cc112xSpiWriteReg(preferredSettings[i].addr, &writeByte, 1);
  }
}

uint8_t transceiverReadStatus()
{
  uint8_t data;
  data=trxSpiCmdStrobe(CC112X_SNOP);
  return data;
}

uint8_t transceiverReadReg(uint16_t addr)
{
  uint8_t data;
  cc112xSpiReadReg(addr, &data, 1);
  return data;
}

/*******************************************************************************
*   @fn         manualCalibration
*
*   @brief      Calibrates radio according to CC112x errata
*
*   @param      none
*
*   @return     none
*/
#define VCDAC_START_OFFSET 2
#define FS_VCO2_INDEX 0
#define FS_VCO4_INDEX 1
#define FS_CHP_INDEX 2
void transceiverManualCalibration(void) 
{
    uint8_t original_fs_cal2;
    uint8_t calResults_for_vcdac_start_high[3];
    uint8_t calResults_for_vcdac_start_mid[3];
    uint8_t marcstate;
    uint8_t writeByte;

    // 1) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
    writeByte = 0x00;
    cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);

    // 2) Start with high VCDAC (original VCDAC_START + 2):
    cc112xSpiReadReg(CC112X_FS_CAL2, &original_fs_cal2, 1);
    writeByte = original_fs_cal2 + VCDAC_START_OFFSET;
    cc112xSpiWriteReg(CC112X_FS_CAL2, &writeByte, 1);

    // 3) Calibrate and wait for calibration to be done
    //   (radio back in IDLE state)
    trxSpiCmdStrobe(CC112X_SCAL);

    do {
        cc112xSpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
    } while (marcstate != 0x41);

    // 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with 
    //    high VCDAC_START value
    cc112xSpiReadReg(CC112X_FS_VCO2,
                     &calResults_for_vcdac_start_high[FS_VCO2_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_VCO4,
                     &calResults_for_vcdac_start_high[FS_VCO4_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_CHP,
                     &calResults_for_vcdac_start_high[FS_CHP_INDEX], 1);

    // 5) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
    writeByte = 0x00;
    cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);

    // 6) Continue with mid VCDAC (original VCDAC_START):
    writeByte = original_fs_cal2;
    cc112xSpiWriteReg(CC112X_FS_CAL2, &writeByte, 1);

    // 7) Calibrate and wait for calibration to be done
    //   (radio back in IDLE state)
    trxSpiCmdStrobe(CC112X_SCAL);

    do {
        cc112xSpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
    } while (marcstate != 0x41);

    // 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained 
    //    with mid VCDAC_START value
    cc112xSpiReadReg(CC112X_FS_VCO2, 
                     &calResults_for_vcdac_start_mid[FS_VCO2_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_VCO4,
                     &calResults_for_vcdac_start_mid[FS_VCO4_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_CHP,
                     &calResults_for_vcdac_start_mid[FS_CHP_INDEX], 1);

    // 9) Write back highest FS_VCO2 and corresponding FS_VCO
    //    and FS_CHP result
    if (calResults_for_vcdac_start_high[FS_VCO2_INDEX] >
        calResults_for_vcdac_start_mid[FS_VCO2_INDEX]) {
        writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
        cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
    } else {
        writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
        cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
    }
}

void transceiverSendPacket(uint8_t txBuffer[], uint8_t len)
{
  createPacket(txBuffer, len);
  cc112xSpiWriteTxFifo(txBuffer, len+1);
  trxSpiCmdStrobe(CC112X_STX);
  
  while(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)){}
  while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)){}
}

void createPacket(uint8_t txBuffer[], uint8_t len)
{
    txBuffer[0] = len;             // Length byte - MUST BE Length of Packet -1
    txBuffer[1] = getMemory(ADDR_SLAVE_ID);
    txBuffer[2] = getMemory(ADDR_MASTER_ID);
}

void transceiverRXOn()
{
  trxSpiCmdStrobe(CC112X_SRX);
}

void transceiverReadPacket()
{
  uint8_t rxBuffer[255] = {0};
  uint8_t rxBytes;
  uint8_t marcState;
  
  marcState=cc112xSpiReadReg(CC112X_NUM_RXBYTES, &rxBytes, 1);
  if(rxBytes!=0)
  {
    while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)){}
    marcState=cc112xSpiReadReg(CC112X_NUM_RXBYTES, &rxBytes, 1);
    cc112xSpiReadReg(CC112X_MARCSTATE, &marcState, 1);
    if((marcState & 0x1F) == RX_FIFO_ERROR) 
    {
      trxSpiCmdStrobe(CC112X_SFRX);
    } else 
    {
      cc112xSpiReadRxFifo(rxBuffer, rxBytes);
      if(rxBuffer[2]==getMemory(ADDR_SLAVE_ID))
      {
        transceiverExecutor(rxBuffer);
      }
      // Set radio back in RX
      trxSpiCmdStrobe(CC112X_SRX);
    }
  }
}

void transceiverExecutor(uint8_t buffer[])
{
  uint32_t temp32;
  uint16_t temp;
  uint8_t  i;
  
  if(buffer[3]==0x01) //set SLAVE_ID
  {
    setMemory(ADDR_SLAVE_ID,buffer[4]);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x02) //set MASTER_ID
  {
    setMemory(ADDR_MASTER_ID,buffer[4]);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x03) //set Mode
  {
    if(buffer[4]==1) setMemory(ADDR_MSR_NUM, 0);
    setMemory(ADDR_MODE,buffer[4]);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x04) //set Current Time
  {
    temp32=(buffer[4]<<24|buffer[5]<<16|buffer[6]<<8|buffer[7]);
    setRTCtime(temp32);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x05) //set nearest time to Measure
  {
    temp32=(buffer[4]<<24|buffer[5]<<16|buffer[6]<<8|buffer[7]);
    setMemory(ADDR_MSR_TIME,temp32);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x06) //set Measure interval
  {
    setMemory(ADDR_MSR_INT,buffer[4]*60);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x07) //set Measure count
  {
    setMemory(ADDR_MSR_CNT,buffer[4]);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x08) //set nearest time to Transmit
  {
    temp32=(buffer[4]<<24|buffer[5]<<16|buffer[6]<<8|buffer[7]);
    setMemory(ADDR_TRM_TIME,temp32);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x09) //set start measure number
  {
    temp32=(buffer[4]<<8|buffer[5]);
    setMemory(ADDR_MSR_NUM,temp32);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x10) //set tsensor's number and address
  {
    setMemory(ADDR_TSENSOR_NUM, buffer[4]);
    for(i=0; i<buffer[4]; i++)
    {
      temp32=((buffer[5+i*8]<<24)|(buffer[6+i*8]<<16)|(buffer[7+i*8]<<8)|(buffer[8+i*8]));
      setMemory(ADDR_TSENSOR_NUM+4+i*8, temp32);
      temp32=((buffer[9+i*8]<<24)|(buffer[10+i*8]<<16)|(buffer[11+i*8]<<8)|(buffer[12+i*8]));
      setMemory(ADDR_TSENSOR_NUM+8+i*8, temp32);
    }
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x12) //set TEST flag
  {
    setMemory(ADDR_TEST_FLAG, buffer[4]);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x13) //set STOP flag
  {
    setMemory(ADDR_STOP_FLAG, 1);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x15) //set nearest time to Measure
  {
    temp32=(buffer[4]<<24|buffer[5]<<16|buffer[6]<<8|buffer[7]);
    setMemory(ADDR_CAL_TIME,temp32);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x18) //set nearest time to Transmit
  {
    temp32=(buffer[4]<<24|buffer[5]<<16|buffer[6]<<8|buffer[7]);
    setMemory(ADDR_CTR_TIME,temp32);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x19) //set start calibration number
  {
    temp32=(buffer[4]<<8|buffer[5]);
    setMemory(ADDR_CAL_NUM,temp32);
    transceiverSendOk(buffer);
  } else
  if(buffer[3]==0x20) //get tsensor's number and address
  {
    uw_search_term();
    buffer[4]=(getMemory(ADDR_TSENSOR_NUM));
    for(i=0; i<buffer[4]; i++)
    {
      buffer[5+i*8]=(getMemory(ADDR_TSENSOR_NUM+4+i*8)>>24)&0xFF;
      buffer[6+i*8]=(getMemory(ADDR_TSENSOR_NUM+4+i*8)>>16)&0xFF;
      buffer[7+i*8]=(getMemory(ADDR_TSENSOR_NUM+4+i*8)>>8)&0xFF;
      buffer[8+i*8]=(getMemory(ADDR_TSENSOR_NUM+4+i*8)>>0)&0xFF;
      buffer[9+i*8]=(getMemory(ADDR_TSENSOR_NUM+8+i*8)>>24)&0xFF;
      buffer[10+i*8]=(getMemory(ADDR_TSENSOR_NUM+8+i*8)>>16)&0xFF;
      buffer[11+i*8]=(getMemory(ADDR_TSENSOR_NUM+8+i*8)>>8)&0xFF;
      buffer[12+i*8]=(getMemory(ADDR_TSENSOR_NUM+8+i*8)>>0)&0xFF;
    }
    transceiverSendPacket(buffer, 4+(buffer[4])*8);
  } else
  if(buffer[3]==0x21) //get Packet of Data
  {
    uint8_t stat_nop = (getMemory(ADDR_TEST_FLAG)?STAT_NOP_T:STAT_NOP);
    uint8_t nop=((getMemory(ADDR_MSR_NUM)>>8)+stat_nop-1)%stat_nop;
    for(i=0; i<(getMemory(ADDR_MSR_CNT)+1); i++)
    {
      buffer[4+i*4]=(getMemory(ADDR_TEMP_MEM+nop*(0x80)+i*4)>>24)&0xFF;
      buffer[5+i*4]=(getMemory(ADDR_TEMP_MEM+nop*(0x80)+i*4)>>16)&0xFF;
      buffer[6+i*4]=(getMemory(ADDR_TEMP_MEM+nop*(0x80)+i*4)>>8)&0xFF;
      buffer[7+i*4]=(getMemory(ADDR_TEMP_MEM+nop*(0x80)+i*4))&0xFF;    
    }
    transceiverSendPacket(buffer, 3+(getMemory(ADDR_MSR_CNT)+1)*4);
  } else
  if(buffer[3]==0x22) //get First part of Packet of Calibration Data
  {
    uint8_t calb_nop = (getMemory(ADDR_TEST_FLAG)?CALB_NOP_T:CALB_NOP);
    uint8_t nop=((getMemory(ADDR_CAL_NUM)>>8)+calb_nop-1)%calb_nop;
    
    buffer[4]=(getMemory(ADDR_CAL_MEM+nop*(0x80))>>24)&0xFF;
    buffer[5]=(getMemory(ADDR_CAL_MEM+nop*(0x80))>>16)&0xFF;
    buffer[6]=(getMemory(ADDR_CAL_MEM+nop*(0x80))>>8)&0xFF;
    buffer[7]=(getMemory(ADDR_CAL_MEM+nop*(0x80)))&0xFF;
    
    for(i=0; i<(CALB_NOM/2); i++)
    {
      buffer[8+i*4]=(getMemory(ADDR_CAL_MEM+nop*(0x80)+(i+1)*4)>>24)&0xFF;
      buffer[9+i*4]=(getMemory(ADDR_CAL_MEM+nop*(0x80)+(i+1)*4)>>16)&0xFF;
      buffer[10+i*4]=(getMemory(ADDR_CAL_MEM+nop*(0x80)+(i+1)*4)>>8)&0xFF;
      buffer[11+i*4]=(getMemory(ADDR_CAL_MEM+nop*(0x80)+(i+1)*4))&0xFF;    
    }
    transceiverSendPacket(buffer, 7+(CALB_NOM/2)*4);
  } else
  if(buffer[3]==0x23) //get Second part of Packet of Calibration Data
  {
    uint8_t calb_nop = (getMemory(ADDR_TEST_FLAG)?CALB_NOP_T:CALB_NOP);
    uint8_t nop=((getMemory(ADDR_CAL_NUM)>>8)+calb_nop-1)%calb_nop;
    
    buffer[4]=(getMemory(ADDR_CAL_MEM+nop*(0x80))>>24)&0xFF;
    buffer[5]=(getMemory(ADDR_CAL_MEM+nop*(0x80))>>16)&0xFF;
    buffer[6]=(getMemory(ADDR_CAL_MEM+nop*(0x80))>>8)&0xFF;
    buffer[7]=(getMemory(ADDR_CAL_MEM+nop*(0x80)))&0xFF;
    
    for(i=0; i<(CALB_NOM/2); i++)
    {
      buffer[8+i*4]=(getMemory(ADDR_CAL_MEM+nop*(0x80)+((CALB_NOM/2)+(i+1))*4)>>24)&0xFF;
      buffer[9+i*4]=(getMemory(ADDR_CAL_MEM+nop*(0x80)+((CALB_NOM/2)+(i+1))*4)>>16)&0xFF;
      buffer[10+i*4]=(getMemory(ADDR_CAL_MEM+nop*(0x80)+((CALB_NOM/2)+(i+1))*4)>>8)&0xFF;
      buffer[11+i*4]=(getMemory(ADDR_CAL_MEM+nop*(0x80)+((CALB_NOM/2)+(i+1))*4))&0xFF;    
    }
    transceiverSendPacket(buffer, 7+(CALB_NOM/2)*4);
  } else
  if(buffer[3]==0x24) //get Current Time
  {    
    temp32=getRTCtime();
    buffer[4]=(uint8_t)(temp32>>24);
    buffer[5]=(uint8_t)(temp32>>16);
    buffer[6]=(uint8_t)(temp32>>8);
    buffer[7]=(uint8_t)temp32;
    transceiverSendPacket(buffer, 7);
  } else
  if(buffer[3]==0x31) //get Current 1-Wire Temperature
  {
    temp32=uw_get_term()<<3;
    buffer[4]=temp32>>24;
    buffer[5]=temp32>>16;
    buffer[6]=temp32>>8;
    buffer[7]=temp32&0xFF;
    transceiverSendPacket(buffer, 7);
  } else
  if(buffer[3]==0x32) //get Packet of Data from Slave
  {
    uint8_t nop=buffer[4];
    for(i=0; i<(getMemory(ADDR_MSR_CNT)+1); i++)
    {
      buffer[4+i*4]=(getMemory(ADDR_TEMP_MEM+nop*(0x80)+i*4)>>24)&0xFF;
      buffer[5+i*4]=(getMemory(ADDR_TEMP_MEM+nop*(0x80)+i*4)>>16)&0xFF;
      buffer[6+i*4]=(getMemory(ADDR_TEMP_MEM+nop*(0x80)+i*4)>>8)&0xFF;
      buffer[7+i*4]=(getMemory(ADDR_TEMP_MEM+nop*(0x80)+i*4))&0xFF;    
    }
    transceiverSendPacket(buffer, 3+(getMemory(ADDR_MSR_CNT)+1)*4);
  } else
  if(buffer[3]==0x33) //get Calibration Packet from Slave
  {
    uint8_t nop=buffer[4];
    for(i=0; i<(CALB_NOM+1); i++)
    {
      buffer[4+i*4]=(getMemory(ADDR_CAL_MEM+nop*(0x80)+i*4)>>24)&0xFF;
      buffer[5+i*4]=(getMemory(ADDR_CAL_MEM+nop*(0x80)+i*4)>>16)&0xFF;
      buffer[6+i*4]=(getMemory(ADDR_CAL_MEM+nop*(0x80)+i*4)>>8)&0xFF;
      buffer[7+i*4]=(getMemory(ADDR_CAL_MEM+nop*(0x80)+i*4))&0xFF;    
    }
    transceiverSendPacket(buffer, 3+(CALB_NOM+1)*4);
  } else
  if(buffer[3]==0xF1) //go to Sleep
  {
    transceiverSendOk(buffer);
    transceiverSleep();
    gotoSleep();
  } else
  if(buffer[3]==0xF2) //immediate change of mode 
  {
    setMemory(ADDR_MODE,buffer[4]);
    transceiverSendOk(buffer);
    transceiverSleep();
    reload();
  }
  
  
  
  
  
    
    else
  if(buffer[3]==0x41) //get Power
  {    
    temp=transceiverReadReg(CC112X_RSSI1)<<4;
    i=transceiverReadReg(CC112X_RSSI0)>>3;
    temp=temp|(i&0x0F);
    buffer[4]=(uint8_t)(temp>>8);
    buffer[5]=(uint8_t)temp;
    transceiverSendPacket(buffer, 5);
  } else
  if(buffer[3]==0x42) //set transmit power
  {
    temp=buffer[4];
    transceiverSendOk(buffer);
    transceiverChangeReg(20, (temp+17)*2-1);
    transceiverConfig();
    transceiverManualCalibration();
    transceiverRXOn();
  } else
  if(buffer[3]==0x43) //set speed
  {
    temp=buffer[4];
    transceiverSendOk(buffer);
    transceiverChangeReg(10, temp);
    transceiverConfig();
    transceiverManualCalibration();
    transceiverRXOn();
  } else
  if(buffer[3]==0x44) //set modulation
  {
    temp=buffer[4];
    transceiverSendOk(buffer);
    if(temp==1) transceiverChangeReg(5, 0x03);
    if(temp==2) transceiverChangeReg(5, 0x0B);
    if(temp==3) transceiverChangeReg(5, 0x23);
    if(temp==4) transceiverChangeReg(5, 0x2B);
    if(temp%2) transceiverChangeReg(4, 0x0B);
    else transceiverChangeReg(4, 0x08);
    transceiverConfig();
    transceiverManualCalibration();
    transceiverRXOn();
  } else
  if(buffer[3]==0x45) //for measure of current
  {
    temp=buffer[4];
    while(1){transceiverSendOk(buffer);}
  } 
  
  
  
  
    else //unknown command
  {
    buffer[4]=0xE0;
    buffer[5]=0xE1;
    transceiverSendPacket(buffer, 5);
  }
}

void transceiverSendOk(uint8_t buffer[])
{
  buffer[4]='o';
  buffer[5]='k';
  transceiverSendPacket(buffer, 5);
}

void transceiverChangeReg(uint8_t num, uint8_t val)
{
  preferredSettings[num].data=val;
}