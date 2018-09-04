#include "uartwire.h"
#include "stm32l1xx_it.h"

uint64_t way;

void uw_init_term()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  //������������� 1-wire-������
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;//|GPIO_Pin_3;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  USART_HalfDuplexCmd(USART2, ENABLE);
  
  //������������� USART
  USART_StructInit(&USART_InitStruct);
  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStruct);
  USART_Cmd(USART2, ENABLE);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  
  uw_send_presence();
  uw_writebyte(0xCC);
  uw_writebyte(0x4E);
  uw_writebyte(0x4B);
  uw_writebyte(0x46);
  uw_writebyte(0x5F);
}

uint8_t uw_send_presence() 
{ 	
  USART_InitTypeDef USART_InitStruct;
  uint8_t bit=0;
  uint8_t rec;
  
  USART_Cmd(USART2, DISABLE);
  USART_DeInit(USART2);
  USART_HalfDuplexCmd(USART2, ENABLE);
  USART_StructInit(&USART_InitStruct);
  USART_InitStruct.USART_BaudRate = 9600;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStruct);
  USART_Cmd(USART2, ENABLE);
  
  USART_ClearFlag(USART2, USART_FLAG_RXNE);
  USART_SendData(USART2, 0xF0);
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
  while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET){}
  rec=USART_ReceiveData(USART2);
  USART_ClearFlag(USART2, USART_FLAG_RXNE);
  if((rec==0xE0)||(rec==0xC0)) bit=1;
  
  USART_Cmd(USART2, DISABLE);
  USART_DeInit(USART2);
  USART_HalfDuplexCmd(USART2, ENABLE);
  USART_StructInit(&USART_InitStruct);
  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStruct);
  USART_Cmd(USART2, ENABLE);
  
  return bit;
}

uint8_t uw_search_term(void)
{
  uint8_t a;
  uint8_t b;
  uint8_t step;
  uint8_t conf[64];
  uint8_t conflen;
  uint8_t way[64];
  uint64_t devcode[10];
  uint8_t devcnt;
  conflen=0;
  devcnt=0;
  
  do
  {
    devcode[devcnt]=0;
    if(!(uw_send_presence())) return 0;
    uw_writebyte(0xF0);
    for(step=0;step<64; step++)
    {
      a=uw_readbit();
      b=uw_readbit();
      if(a==b)
      {
        if((step>conf[conflen-1])||(conflen==0))
        {
          conf[conflen]=step;
          conflen++;
        }
        else
        {
          if(step==conf[conflen-1]) {conflen--; a=1;}
          else {a=way[step];}
        }
      }
      if(a) uw_writebit(1);
      else uw_writebit(0);
      way[step]=a;
      devcode[devcnt]|=((uint64_t)a)<<step;
    }
    devcnt++;
  } while(conflen);
  
  return(devcnt);
}

uint32_t uw_get_term()
{
  uint8_t i;
  
  uw_send_presence();
  uw_writebyte(0x55);
  uw_writebyte(0x28);
  uw_writebyte(0x49);
  uw_writebyte(0x36);
  uw_writebyte(0x53);
  uw_writebyte(0x07);
  uw_writebyte(0x00);
  uw_writebyte(0x00);
  uw_writebyte(0xCF);
  uw_writebyte(0x44);
  
  uw_send_presence();
  uw_writebyte(0x55);
  uw_writebyte(0x28);
  uw_writebyte(0xDE);
  uw_writebyte(0xA8);
  uw_writebyte(0x52);
  uw_writebyte(0x07);
  uw_writebyte(0x00);
  uw_writebyte(0x00);
  uw_writebyte(0x61);
  uw_writebyte(0x44);
  
  uw_send_presence();
  uw_writebyte(0x55);
  uw_writebyte(0x28);
  uw_writebyte(0x49);
  uw_writebyte(0x36);
  uw_writebyte(0x53);
  uw_writebyte(0x07);
  uw_writebyte(0x00);
  uw_writebyte(0x00);
  uw_writebyte(0xCF);
  uw_writebyte(0xBE);
  
  uint32_t data = 0;
  for(i = 0; i<2; i++) data |= (uw_readbyte()<<(8*i));
  
  uw_send_presence();
  uw_writebyte(0x55);
  uw_writebyte(0x28);
  uw_writebyte(0xDE);
  uw_writebyte(0xA8);
  uw_writebyte(0x52);
  uw_writebyte(0x07);
  uw_writebyte(0x00);
  uw_writebyte(0x00);
  uw_writebyte(0x61);
  uw_writebyte(0xBE);
  
  for(i = 2; i<4; i++) data |= (uw_readbyte()<<(8*i));
  
  return data;    
}

uint8_t uw_readbit()
{
  uint8_t bit=0;
  
  USART_ClearFlag(USART2, USART_FLAG_RXNE);
  USART_SendData(USART2, 0xFF);
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
  while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET){}
  if(USART_ReceiveData(USART2)==0xFF) bit=1;
  USART_ClearFlag(USART2, USART_FLAG_RXNE);
  
  return bit;
}

uint8_t uw_readbyte()
{
  uint8_t bit;
  uint8_t i;
  
  bit=0;
  
  for(i=0; i<8; i++)
  {
    if(uw_readbit()) bit|=1<<i;
  }
  return bit;
}

void uw_writebit(uint8_t bit)
{
  USART_ClearFlag(USART2, USART_FLAG_RXNE);
  USART_SendData(USART2, (bit ? 0xFF : 0x00));
  while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET){}
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
  USART_ReceiveData(USART2);
  USART_ClearFlag(USART2, USART_FLAG_RXNE);
}

void uw_writebyte(uint8_t data)
{
  uint8_t i;
  
  for(i=0; i<8; i++)
  {
    uw_writebit((data>>i)&0x01);
  }
}

