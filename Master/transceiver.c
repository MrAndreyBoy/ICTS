#include "transceiver.h"
#include "cc112x_spi.h"
#include "cc112x_easy_link_reg_config.h"
#include "stm32l1xx_it.h"
#include "memory.h"
#include "masterwork.h"

uint8_t MASTER_ID;

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

uint8_t transceiverRead()
{
  uint8_t data;
  cc112xSpiReadReg(CC112X_NUM_TXBYTES, &data, 1);
  return data;
}

/*******************************************************************************
*   @fn         transceiverManualCalibration
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
  txBuffer[0] = len;           // Length byte - MUST Length of packet-1
  txBuffer[1] = MASTER_ID;
}

void transceiverRXOn()
{
  trxSpiCmdStrobe(CC112X_SRX);
}

uint8_t transceiverReadPacket(uint8_t * rxBuffer)
{
  uint8_t rxBytes;
  uint8_t marcState;
  uint8_t i;
  uint16_t sTime;  
  
  sTime=getTime();
  marcState=cc112xSpiReadReg(CC112X_NUM_RXBYTES, &rxBytes, 1);
  while((rxBytes==0)&&(Delay(sTime)<5000))
  {
    marcState=cc112xSpiReadReg(CC112X_NUM_RXBYTES, &rxBytes, 1);
  }
  while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
  {}
  marcState=cc112xSpiReadReg(CC112X_NUM_RXBYTES, &rxBytes, 1);
  if(rxBytes != 0) 
  {
    // Read MARCSTATE to check for RX FIFO error
    cc112xSpiReadReg(CC112X_MARCSTATE, &marcState, 1);
    // Mask out MARCSTATE bits and check if we have a RX FIFO error
    if((marcState & 0x1F) == RX_FIFO_ERROR) 
    {
      // Flush RX FIFO
      trxSpiCmdStrobe(CC112X_SFRX);
    } else 
    {
      // Read n bytes from RX FIFO
      cc112xSpiReadRxFifo(rxBuffer, rxBytes);
      /*USART_SendData(USART3, (rxBytes));
      while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
      {}
      for(i=0; i<rxBytes; i++)
      {
        USART_SendData(USART3, (rxBuffer[i]));
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        {}
      }*/
      trxSpiCmdStrobe(CC112X_SRX);
      return rxBytes;
    }
  }
  trxSpiCmdStrobe(CC112X_SRX);
  return 0;
}

void transceiverReadPacket_mode()
{
  uint8_t rxBuffer[256] = {0};
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
      //if(rxBuffer[2]==getMemory(ADDR_MASTER_ID))
      //{
        MasterWork(rxBuffer);
      //}
      // Set radio back in RX
      trxSpiCmdStrobe(CC112X_SRX);
    }
  }
}

void transceiverChangeReg(uint8_t num, uint8_t val)
{
  preferredSettings[num].data=val;
}