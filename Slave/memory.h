/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MEMORY_H
#define __MEMORY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/\
/* Exported macro ------------------------------------------------------------*/

#define ADDR_SLAVE_ID       0x08080000
#define ADDR_MASTER_ID      0x08080004
#define ADDR_MODE           0x08080008
#define ADDR_MSR_TIME       0x0808000C
#define ADDR_MSR_INT        0x08080010
#define ADDR_MSR_CNT        0x08080014
#define ADDR_MSR_NUM        0x08080018
#define ADDR_TRM_TIME       0x0808001C
#define ADDR_CAL_TIME       0x08080020
#define ADDR_CTR_TIME       0x08080024
#define ADDR_CAL_NUM        0x08080028
#define ADDR_STOP_FLAG      0x0808002C
#define ADDR_TEST_FLAG      0x08080030
#define ADDR_TSENSOR_NUM    0x08080034
#define ADDR_TEMP_MEM       0x08080100
#define ADDR_CAL_MEM        0x08080500
  
#define STAT_NOM            24//8
#define STAT_IOM            (60*60)//3
#define STAT_NOP            7
#define STAT_IOT            (24*60*60)
#define CALB_NOM            30//3
#define CALB_IOM            60
#define CALB_NOP            6//3
#define CALB_IOT            (60*30)//3

#define STAT_NOM_T          8
#define STAT_IOM_T          (60*3)
#define STAT_NOP_T          7
#define STAT_IOT_T          (24*60)
#define CALB_NOM_T          3
#define CALB_IOM_T          60
#define CALB_NOP_T          3
#define CALB_IOT_T          (60*3)
  
/* Exported functions ------------------------------------------------------- */
void setMemory(uint32_t addr, uint32_t value);
uint32_t getMemory(uint32_t addr);
//void initMemory();
#endif /* __MEMORY_H */
