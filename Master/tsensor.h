/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TSENSOR_H
#define __TSENSOR_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

uint16_t ReadTemp(uint8_t address);

#endif /* __TSENSOR_H */
