/*****************************************************************************/
// @file 	cc120x_spi.c  
//    
// @brief 	Implementation file for basic and neccessary functions
//          to communicate with CC120X over SPI
//				 
//  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/****************************************************************************/


/******************************************************************************
 * INCLUDES
 */
#include "cc120x_spi.h"

/******************************************************************************
 * FUNCTIONS
 */

/******************************************************************************
 * @fn          cc120xSpiReadReg
 *
 * @brief       Read value(s) from config/status/extended radio register(s).
 *              If len  = 1: Reads a single register
 *              if len != 1: Reads len register values in burst mode 
 *
 * input parameters
 *
 * @param       addr   - address of first register to read
 * @param       *pData - pointer to data array where read bytes are saved
 * @param       len   - number of bytes to read
 *
 * output parameters
 *
 * @return      rfStatus_t
 */
rfStatus_t cc120xSpiReadReg(uint16_t addr, uint8_t *pData, uint8_t len)
{
  uint8_t tempExt  = (uint8_t)(addr>>8);
  uint8_t tempAddr = (uint8_t)(addr & 0x00FF);
  uint8_t rc;
  
  /* Checking if this is a FIFO access -> returns chip not ready  */
  if((CC120X_SINGLE_TXFIFO<=tempAddr)&&(tempExt==0)) return STATUS_CHIP_RDYn_BM;
  
  /* Decide what register space is accessed */
  if(!tempExt)
  {
    rc = trx8BitRegAccess((RADIO_BURST_ACCESS|RADIO_READ_ACCESS),tempAddr,pData,len);
  }
  else if (tempExt == 0x2F)
  {
    rc = trx16BitRegAccess((RADIO_BURST_ACCESS|RADIO_READ_ACCESS),tempExt,tempAddr,pData,len);
  }
  return (rc);
}

/******************************************************************************
 * @fn          cc120xSpiWriteReg
 *
 * @brief       Write value(s) to config/status/extended radio register(s).
 *              If len  = 1: Writes a single register
 *              if len  > 1: Writes len register values in burst mode 
 *
 * input parameters
 *
 * @param       addr   - address of first register to write
 * @param       *pData - pointer to data array that holds bytes to be written
 * @param       len    - number of bytes to write
 *
 * output parameters
 *
 * @return      rfStatus_t
 */
rfStatus_t cc120xSpiWriteReg(uint16_t addr, uint8_t *pData, uint8_t len)
{
  uint8_t tempExt  = (uint8_t)(addr>>8);
  uint8_t tempAddr = (uint8_t)(addr & 0x00FF);
  uint8_t rc;
  
  /* Checking if this is a FIFO access - returns chip not ready */
  if((CC120X_SINGLE_TXFIFO<=tempAddr)&&(tempExt==0)) return STATUS_CHIP_RDYn_BM;
  	
  /* Decide what register space is accessed */  
  if(!tempExt)
  {
    rc = trx8BitRegAccess((RADIO_BURST_ACCESS|RADIO_WRITE_ACCESS),tempAddr,pData,len);
  }
  else if (tempExt == 0x2F)
  {
    rc = trx16BitRegAccess((RADIO_BURST_ACCESS|RADIO_WRITE_ACCESS),tempExt,tempAddr,pData,len);
  }
  return (rc);
}

/*******************************************************************************
 * @fn          cc120xSpiWriteTxFifo
 *
 * @brief       Write pData to radio transmit FIFO.
 *
 * input parameters
 *
 * @param       *pData - pointer to data array that is written to TX FIFO
 * @param       len    - Length of data array to be written
 *
 * output parameters
 *
 * @return      rfStatus_t
 */
rfStatus_t cc120xSpiWriteTxFifo(uint8_t *pData, uint8_t len)
{
  uint8_t rc;
  rc = trx8BitRegAccess(0x00,CC120X_BURST_TXFIFO, pData, len);
  return (rc);
}

/*******************************************************************************
 * @fn          cc120xSpiReadRxFifo
 *
 * @brief       Reads RX FIFO values to pData array
 *
 * input parameters
 *
 * @param       *pData - pointer to data array where RX FIFO bytes are saved
 * @param       len    - number of bytes to read from the RX FIFO
 *
 * output parameters
 *
 * @return      rfStatus_t
 */
rfStatus_t cc120xSpiReadRxFifo(uint8_t * pData, uint8_t len)
{
  uint8_t rc;
  rc = trx8BitRegAccess(0x00,CC120X_BURST_RXFIFO, pData, len);
  return (rc);
}

/******************************************************************************
 * @fn      cc120xGetTxStatus(void)
 *          
 * @brief   This function transmits a No Operation Strobe (SNOP) to get the 
 *          status of the radio and the number of free bytes in the TX FIFO.
 *          
 *          Status byte:
 *          
 *          ---------------------------------------------------------------------------
 *          |          |            |                                                 |
 *          | CHIP_RDY | STATE[2:0] | FIFO_BYTES_AVAILABLE (free bytes in the TX FIFO |
 *          |          |            |                                                 |
 *          ---------------------------------------------------------------------------
 *
 *
 * input parameters
 *
 * @param   none
 *
 * output parameters
 *         
 * @return  rfStatus_t 
 *
 */
rfStatus_t cc120xGetTxStatus(void)
{
    return(trxSpiCmdStrobe(CC120X_SNOP));
}

/******************************************************************************
 *
 *  @fn       cc120xGetRxStatus(void)
 *
 *  @brief   
 *            This function transmits a No Operation Strobe (SNOP) with the 
 *            read bit set to get the status of the radio and the number of 
 *            available bytes in the RXFIFO.
 *            
 *            Status byte:
 *            
 *            --------------------------------------------------------------------------------
 *            |          |            |                                                      |
 *            | CHIP_RDY | STATE[2:0] | FIFO_BYTES_AVAILABLE (available bytes in the RX FIFO |
 *            |          |            |                                                      |
 *            --------------------------------------------------------------------------------
 *
 *
 * input parameters
 *
 * @param     none
 *
 * output parameters
 *         
 * @return    rfStatus_t 
 *
 */
rfStatus_t cc120xGetRxStatus(void)
{
    return(trxSpiCmdStrobe(CC120X_SNOP | RADIO_READ_ACCESS));
}                                                            

/*******************************************************************************
 * @fn          trx8BitRegAccess
 *
 * @brief       This function performs a read or write from/to a 8bit register
 *              address space. The function handles burst and single read/write
 *              as specfied in addrByte. Function assumes that chip is ready.
 *
 * input parameters
 *
 * @param       accessType - Specifies if this is a read or write and if it's
 *                           a single or burst access. Bitmask made up of
 *                           RADIO_BURST_ACCESS/RADIO_SINGLE_ACCESS/
 *                           RADIO_WRITE_ACCESS/RADIO_READ_ACCESS.
 * @param       addrByte - address byte of register.
 * @param       pData    - data array
 * @param       len      - Length of array to be read(TX)/written(RX)
 *
 * output parameters
 *
 * @return      chip status
 */
rfStatus_t trx8BitRegAccess(uint8_t accessType, uint8_t addrByte, uint8_t *pData, uint16_t len)
{
  uint8_t readValue = 0;
  uint8_t writeValue;
  uint8_t i;
  
  /* Pull CS_N low and wait for SO to go low before communication starts */
  GPIO_ResetBits(GPIOA, GPIO_Pin_4);
  while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6));
  /* send register address byte */
  writeValue=accessType|addrByte;
  for(i=0;i<8;i++)
  {
    if(writeValue&0x80) GPIO_SetBits(GPIOA, GPIO_Pin_7);
    else GPIO_ResetBits(GPIOA, GPIO_Pin_7);
    readValue=readValue<<1;
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
    writeValue=writeValue<<1;
    readValue|=GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
    GPIO_ResetBits(GPIOA,GPIO_Pin_5);
  }
  /* Storing chip status */
  trxReadWriteBurstSingle(accessType|addrByte,pData,len);
  GPIO_SetBits(GPIOA, GPIO_Pin_4);
  /* return the status byte value */
  return(readValue);
}

/******************************************************************************
 * @fn          trx16BitRegAccess
 *
 * @brief       This function performs a read or write in the extended adress
 *              space of CC112X.
 *
 * input parameters
 *
 * @param       accessType - Specifies if this is a read or write and if it's
 *                           a single or burst access. Bitmask made up of
 *                           RADIO_BURST_ACCESS/RADIO_SINGLE_ACCESS/
 *                           RADIO_WRITE_ACCESS/RADIO_READ_ACCESS.
 * @param       extAddr - Extended register space address = 0x2F.
 * @param       regAddr - Register address in the extended address space.
 * @param       *pData  - Pointer to data array for communication
 * @param       len     - Length of bytes to be read/written from/to radio
 *
 * output parameters
 *
 * @return      rfStatus_t
 */
rfStatus_t trx16BitRegAccess(uint8_t accessType, uint8_t extAddr, uint8_t regAddr, uint8_t *pData, uint8_t len)
{
  uint8_t readValue = 0;
  uint8_t writeValue;
  uint8_t i;
  
  GPIO_ResetBits(GPIOA, GPIO_Pin_4);
  while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6));
  /* send extended address byte with access type bits set */
  writeValue=accessType|extAddr;
  for(i=0;i<8;i++)
  {
    if(writeValue&0x80) GPIO_SetBits(GPIOA, GPIO_Pin_7);
    else GPIO_ResetBits(GPIOA, GPIO_Pin_7);
    readValue=readValue<<1;
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
    writeValue=writeValue<<1;
    readValue|=GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
    GPIO_ResetBits(GPIOA,GPIO_Pin_5);
  }
  /* Storing chip status */
  writeValue=regAddr;
  for(i=0;i<8;i++)
  {
    if(writeValue&0x80) GPIO_SetBits(GPIOA, GPIO_Pin_7);
    else GPIO_ResetBits(GPIOA, GPIO_Pin_7);
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
    writeValue=writeValue<<1;
    GPIO_ResetBits(GPIOA,GPIO_Pin_5);
  }
  /* Communicate len number of bytes */
  trxReadWriteBurstSingle(accessType|extAddr,pData,len);
  GPIO_SetBits(GPIOA, GPIO_Pin_4);
  /* return the status byte value */
  return(readValue);
}

/*******************************************************************************
 * @fn          trxSpiCmdStrobe
 *
 * @brief       Send command strobe to the radio. Returns status byte read
 *              during transfer of command strobe. Validation of provided
 *              is not done. Function assumes chip is ready.
 *
 * input parameters
 *
 * @param       cmd - command strobe
 *
 * output parameters
 *
 * @return      status byte
 */
rfStatus_t trxSpiCmdStrobe(uint8_t cmd)
{
    uint8_t rc;
    uint8_t data;
    uint8_t i;
  
    data=cmd;
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6));
    for(i=0;i<8;i++)
    {
      if(data&0x80) GPIO_SetBits(GPIOA, GPIO_Pin_7);
      else GPIO_ResetBits(GPIOA, GPIO_Pin_7);
      rc=rc<<1;
      GPIO_SetBits(GPIOA, GPIO_Pin_5);
      data=data<<1;
      rc|=GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
      GPIO_ResetBits(GPIOA,GPIO_Pin_5);
    }
    
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
    return(rc);
}
                                                              
/*******************************************************************************
 * @fn          trxReadWriteBurstSingle
 *
 * @brief       When the address byte is sent to the SPI slave, the next byte
 *              communicated is the data to be written or read. The address
 *              byte that holds information about read/write -and single/
 *              burst-access is provided to this function.
 *
 *              Depending on these two bits this function will write len bytes to
 *              the radio in burst mode or read len bytes from the radio in burst
 *              mode if the burst bit is set. If the burst bit is not set, only
 *              one data byte is communicated.
 *
 *              NOTE: This function is used in the following way:
 *
 *              TRXEM_SPI_BEGIN();
 *              while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);
 *              ...[Depending on type of register access]
 *              trxReadWriteBurstSingle(uint8 addr,uint8 *pData,uint16 len);
 *              TRXEM_SPI_END();
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      void
 */
void trxReadWriteBurstSingle(uint8_t addr,uint8_t *pData,uint16_t len)
{
  uint16_t i,j;
  uint8_t readValue = 0;
  uint8_t writeValue;
  
      /* Communicate len number of bytes: if RX - the procedure sends 0x00 to push bytes from slave*/
  if(addr&RADIO_READ_ACCESS)
  {
    if(addr&RADIO_BURST_ACCESS)
    {
      for (j=0; j<len; j++)
      {
          writeValue=0;
          readValue=0;
          for(i=0;i<8;i++)
          {
            if(writeValue&0x80) GPIO_SetBits(GPIOA, GPIO_Pin_7);
            else GPIO_ResetBits(GPIOA, GPIO_Pin_7);
            readValue=readValue<<1;
            GPIO_SetBits(GPIOA, GPIO_Pin_5);
            writeValue=writeValue<<1;
            readValue|=GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
            GPIO_ResetBits(GPIOA,GPIO_Pin_5);
          }
          *pData = readValue;     /* Store pData from last pData RX */
          pData++;
      }
    }
    else
    {
      writeValue=0;
      readValue=0;
      for(i=0;i<8;i++)
      {
        if(writeValue&0x80) GPIO_SetBits(GPIOA, GPIO_Pin_7);
        else GPIO_ResetBits(GPIOA, GPIO_Pin_7);
        readValue=readValue<<1;
        GPIO_SetBits(GPIOA, GPIO_Pin_5);
        writeValue=writeValue<<1;
        readValue|=GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
        GPIO_ResetBits(GPIOA,GPIO_Pin_5);
      }
      *pData = readValue;
    }
  }
  else
  {
    if(addr&RADIO_BURST_ACCESS)
    {
      /* Communicate len number of bytes: if TX - the procedure doesn't overwrite pData */
      for (j=0; j<len; j++)
      {
        writeValue=*pData;
        readValue=0;
        for(i=0;i<8;i++)
        {
          if(writeValue&0x80) GPIO_SetBits(GPIOA, GPIO_Pin_7);
          else GPIO_ResetBits(GPIOA, GPIO_Pin_7);
          readValue=readValue<<1;
          GPIO_SetBits(GPIOA, GPIO_Pin_5);
          writeValue=writeValue<<1;
          readValue|=GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
          GPIO_ResetBits(GPIOA,GPIO_Pin_5);
        }
        pData++;
      }
    }
    else
    {
      writeValue=*pData;
      readValue=0;
      for(i=0;i<8;i++)
      {
        if(writeValue&0x80) GPIO_SetBits(GPIOA, GPIO_Pin_7);
        else GPIO_ResetBits(GPIOA, GPIO_Pin_7);
        readValue=readValue<<1;
        GPIO_SetBits(GPIOA, GPIO_Pin_5);
        writeValue=writeValue<<1;
        readValue|=GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
        GPIO_ResetBits(GPIOA,GPIO_Pin_5);
      }
    }
  }
  return;
}                                                              