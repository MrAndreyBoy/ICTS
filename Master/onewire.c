#include "onewire.h"
#include "stm32l1xx_it.h"

uint8_t send_presence() 
{ 	
  uint8_t bit;
  
  GPIOC->ODR &= ~GPIO_Pin_9;
  sleep1(500);
  GPIOC->ODR |= GPIO_Pin_9;
      
  GPIOC->MODER &= ~((uint32_t)GPIO_Mode_IN<<(2*9));
  GPIOC->PUPDR &= ~((uint32_t)0x11 << (2*9));
  GPIOC->PUPDR |= ((uint32_t)GPIO_PuPd_UP<<(2*9));
    
  sleep1(100);
  bit = ((GPIOC->IDR&GPIO_Pin_9?1:0));
  sleep1(400);
  bit |= ((GPIOC->IDR&GPIO_Pin_9?1:0)<<1);
  
  GPIOC->MODER |= ((uint32_t)GPIO_Mode_OUT<<(2*9));
  
  return bit;
}

void init_term()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  GPIO_SetBits(GPIOC, GPIO_Pin_9);
  send_presence();
  ow_writebyte(0xCC);
  ow_writebyte(0x4E);
  ow_writebyte(0x4B);
  ow_writebyte(0x46);
  ow_writebyte(0x7F);
  
  //USART_SendData(USART1, (uint8_t)(0x12));
  //while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
  
}

uint16_t get_term()
{
  uint8_t i;
  uint16_t data = 0;
  
  send_presence();
  ow_writebyte(0xCC);
  ow_writebyte(0x44);
  
  //USART_SendData(USART1, (uint8_t)(0x34));
  //while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
  
  sleep1(700000);
  
  //USART_SendData(USART1, (uint8_t)(0x56));
  //while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
  
  send_presence();
  ow_writebyte(0xCC);
  ow_writebyte(0xBE);
  
  for(i = 0; i<2; i++) data |= (ow_readbyte()<<(8*i));
  
  /*USART_SendData(USART1, (uint8_t)(data>>8));
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
  USART_SendData(USART1, (uint8_t)(data));
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}*/
      
  return data;
}

void ow_writebit(uint8_t bit)
{
  GPIO_ResetBits(GPIOC, GPIO_Pin_9);
  //sleep(bit ? 5 : 55);
  sleep1(bit ? 10:60);
  GPIO_SetBits(GPIOC, GPIO_Pin_9);
  //sleep(bit ? 75 : 25);
  sleep1(bit ? 90:40);
}

uint8_t ow_readbyte()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  uint8_t bit = 0;
  uint8_t i;
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOC, &GPIO_InitStruct); 
    
  for(i=0; i<8; i++)
  {
    GPIOC->ODR &= ~GPIO_Pin_9;
    sleep1(10);
    
    GPIOC->MODER &= ~((uint32_t)GPIO_Mode_IN<<(2*9));
    GPIOC->PUPDR &= ~((uint32_t)0x11 << (2*9));
    GPIOC->PUPDR |= ((uint32_t)GPIO_PuPd_UP<<(2*9));
    GPIOC->ODR |= GPIO_Pin_9;
    
    sleep1(10);
    bit |= ((GPIOC->IDR&GPIO_Pin_9?1:0)<<i);
    sleep1(70);
    
    GPIOC->MODER |= ((uint32_t)GPIO_Mode_OUT<<(2*9));
    GPIOC->ODR |= GPIO_Pin_9;
    sleep1(35);
  }
  return bit;
}

void ow_writebyte(uint8_t data)
{
  uint8_t i;
  
  for(i=0; i<8; i++) ow_writebit(data>>i & 1);
}
