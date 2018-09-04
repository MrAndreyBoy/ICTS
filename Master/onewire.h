/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ONEWIRE_H
#define __ONEWIRE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
   
uint8_t send_presence(void);
void init_term(void);
uint16_t get_term(void);
void ow_writebit(uint8_t bit);
uint8_t ow_readbyte(void);
void ow_writebyte(uint8_t data);


#endif /* __ONEWIRE_H */
