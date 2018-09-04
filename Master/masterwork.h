/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MASTERWORK_H
#define __MASTERWORK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern uint8_t slave_id[30];            // slaves' adrresses
extern uint8_t slave_num;               // number of registered slaves
extern uint32_t slave_cal_temp[30][40]; // calibration packets from slaves
extern uint32_t slave_temp[30][30];     // packets from slaves
extern uint8_t slave_temp_num[30];      // number of measurement in one packet +1
extern uint8_t slave_com[30][30];       // deferred commands for slave
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void MasterWork(uint8_t * buffer);


#endif /* __MASTERWORK_H */
