/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MEMORY_H
#define __MEMORY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
//extern uint8_t SLAVE_ID;
//extern uint8_t MASTER_ID;
//extern uint8_t INT_TR;
/* Exported macro ------------------------------------------------------------*/

#define ADDR_MASTER_ID      0x08080000

/* Exported functions ------------------------------------------------------- */
void setMemory(uint32_t addr, uint32_t value);
uint32_t getMemory(uint32_t addr);
//void initMemory();
#endif /* __MEMORY_H */
