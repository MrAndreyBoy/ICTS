/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EXECUTOR_H
#define __EXECUTOR_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define Cmd_ReadTemperature 0xAB
/* Exported functions ------------------------------------------------------- */

void executor();

#endif /* __EXECUTOR_H */
