/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UARTWIRE_H
#define __UARTWIRE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
   
uint8_t uw_send_presence(void);
void uw_init_term(void);
uint8_t uw_search_term(void);
uint32_t uw_get_term(void);
uint8_t uw_readbit(void);
uint8_t uw_readbyte(void);
void uw_writebit(uint8_t bit);
void uw_writebyte(uint8_t data);


#endif /* __UARTWIRE_H */
