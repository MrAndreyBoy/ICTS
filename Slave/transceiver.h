/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TRANSCEIVER_H
#define __TRANSCEIVER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define PKTLEN              2 // 1 < PKTLEN < 126
#define RX_FIFO_ERROR       0x11

/* Exported functions ------------------------------------------------------- */
void transceiverInit();
uint8_t transceiverReset();
uint8_t transceiverSleep();
void transceiverDeInit();
void transceiverConfig();
void transceiverRXOn();
uint8_t transceiverReadStatus();
uint8_t transceiverReadReg(uint16_t addr);
void transceiverSendPacket(uint8_t txBuffer[], uint8_t len);
void transceiverRXOn();
void transceiverManualCalibration(void);
void transceiverReadPacket();
void createPacket(uint8_t txBuffer[], uint8_t len);
void transceiverExecutor(uint8_t buffer[]);
void transceiverSendOk(uint8_t buffer[]);
void transceiverChangeReg(uint8_t num, uint8_t val);

#endif /* __TRANSCEIVER_H */
