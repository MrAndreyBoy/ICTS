/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H
#define __RTC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define MONTHDAY(year,month)  ((year%4) ? mos[month] : vmos[month])
/* Exported functions ------------------------------------------------------- */

void RTC_Config();
void gotoSleep(uint8_t h, uint8_t m, uint8_t s);
void setRTCtime(uint32_t time);
void setRTChour(uint8_t hour);
void setRTCmin(uint8_t min);
void setRTCsec(uint8_t sec);
uint32_t getRTCtime();
uint8_t getRTChour();
uint8_t getRTCmin();
uint8_t getRTCsec();

#endif /* __RTC_H */
