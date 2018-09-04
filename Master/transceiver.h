/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TRANSCEIVER_H
#define __TRANSCEIVER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define PKTLEN              2//30 // 1 < PKTLEN < 126
#define RX_FIFO_ERROR       0x11
#define ADDR_MASTER_ID      0x08080000
/* Exported functions ------------------------------------------------------- */
void transceiverInit();
uint8_t transceiverReset();
void transceiverDeInit();
void transceiverConfig();
uint8_t transceiverReadStatus();
uint8_t transceiverReadReg(uint16_t addr);
uint8_t transceiverRead();
void transceiverManualCalibration(void);
void transceiverSendPacket(uint8_t txBuffer[], uint8_t len);
void transceiverRXOn();
uint8_t transceiverReadPacket(uint8_t * rxBuffer);
void transceiverReadPacket_mode();
void createPacket(uint8_t txBuffer[], uint8_t len);
void transceiverChangeReg(uint8_t num, uint8_t val);

#endif /* __TRANSCEIVER_H */
