#include "tsensor.h"
#include "stm32l1xx_it.h"
#include "transceiver.h"

uint16_t ReadTemp(uint8_t Address)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  uint8_t address = Address;
  uint16_t Buffer = 0;
  uint8_t Ack=0;
  int i;
  
  /*
  //СТАРТ
  GPIO_SetBits(GPIOF, GPIO_Pin_6);
  GPIO_SetBits(GPIOF, GPIO_Pin_7);
  sleep(10);
  GPIO_ResetBits(GPIOF, GPIO_Pin_7);
  sleep(10);
  
  //Отправка адреса
  for(i=0; i<7; i++)
  {
    GPIO_ResetBits(GPIOF, GPIO_Pin_6);
    if(address&0x40) GPIO_SetBits(GPIOF, GPIO_Pin_7);
    else GPIO_ResetBits(GPIOF, GPIO_Pin_7);
    sleep(10);
    GPIO_SetBits(GPIOF, GPIO_Pin_6);
    address=address<<1;
    sleep(10);
  }
  
  //Бит записи
  GPIO_ResetBits(GPIOF, GPIO_Pin_6);
  GPIO_ResetBits(GPIOF, GPIO_Pin_7);
  sleep(10);
  GPIO_SetBits(GPIOF, GPIO_Pin_6);
  sleep(10);
  
  //Считываем подтверждение приёма
  GPIO_ResetBits(GPIOF, GPIO_Pin_6);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  sleep(10);
  GPIO_SetBits(GPIOF, GPIO_Pin_6);
  sleep(10);
  Ack|=GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7);
  GPIO_ResetBits(GPIOF, GPIO_Pin_6);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  sleep(10);
  
  //Отправка адреса регистра
  for(i=0; i<8; i++)
  {
    GPIO_ResetBits(GPIOF, GPIO_Pin_6);
    GPIO_ResetBits(GPIOF, GPIO_Pin_7);
    sleep(10);
    GPIO_SetBits(GPIOF, GPIO_Pin_6);
    sleep(10);
  }
  
  //Считываем подтверждение приёма
  GPIO_ResetBits(GPIOF, GPIO_Pin_6);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  sleep(10);
  GPIO_SetBits(GPIOF, GPIO_Pin_6);
  sleep(10);
  Ack|=GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7);
  GPIO_ResetBits(GPIOF, GPIO_Pin_6);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_SetBits(GPIOF, GPIO_Pin_7);
  sleep(10);*/
  
    
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  //повторяем СТАРТ
  GPIO_SetBits(GPIOB, GPIO_Pin_6);
  GPIO_SetBits(GPIOB, GPIO_Pin_7);
  sleep(10);
  GPIO_ResetBits(GPIOB, GPIO_Pin_7);
  sleep(10);
  
  //Отправка адреса
  address=Address;
  for(i=0; i<7; i++)
  {
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
    if(address&0x40) GPIO_SetBits(GPIOB, GPIO_Pin_7);
    else GPIO_ResetBits(GPIOB, GPIO_Pin_7);
    sleep(10);
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    address=address<<1;
    sleep(10);
  }
    
  //Бит чтения
  GPIO_ResetBits(GPIOB, GPIO_Pin_6);
  GPIO_SetBits(GPIOB, GPIO_Pin_7);
  sleep(10);
  GPIO_SetBits(GPIOB, GPIO_Pin_6);
  sleep(10);
  
  //Считываем подтверждение приёма
  GPIO_ResetBits(GPIOB, GPIO_Pin_6);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  sleep(10);
  GPIO_SetBits(GPIOB, GPIO_Pin_6);
  sleep(10);
  Ack|=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);
  
  //Чтение старшего байта температуры
  for(i=0; i<8; i++)
  {
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
    Buffer=Buffer<<1;
    sleep(10);
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    sleep(10);
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)) Buffer|=0x01;
  }
  
  //Бит подтверждения
  GPIO_ResetBits(GPIOB, GPIO_Pin_6);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_ResetBits(GPIOB, GPIO_Pin_7);
  sleep(10);
  GPIO_SetBits(GPIOB, GPIO_Pin_6);
  sleep(10);
  GPIO_ResetBits(GPIOB, GPIO_Pin_6);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  //Чтение старшего байта температуры
  for(i=0; i<8; i++)
  {
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
    Buffer=Buffer<<1;
    sleep(10);
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    sleep(10);
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)) Buffer|=0x01;
  }
  
  //Бит неподтверждения
  GPIO_ResetBits(GPIOB, GPIO_Pin_6);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_SetBits(GPIOB, GPIO_Pin_7);
  sleep(10);
  GPIO_SetBits(GPIOB, GPIO_Pin_6);
  sleep(10);
  GPIO_ResetBits(GPIOB, GPIO_Pin_6);
  GPIO_ResetBits(GPIOB, GPIO_Pin_7);
  sleep(10);
  
  //Стоп
  GPIO_SetBits(GPIOB, GPIO_Pin_6);
  sleep(10);
  GPIO_SetBits(GPIOB, GPIO_Pin_7);
  sleep(10);
  
  return Buffer;
}