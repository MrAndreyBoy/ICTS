#include "memory.h"

//uint8_t SLAVE_ID;
//uint8_t MASTER_ID;
//uint8_t INT_TR;

void setMemory(uint32_t addr, uint32_t value)
{
  DATA_EEPROM_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
            | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
  *(__IO uint32_t*)addr=value;
  DATA_EEPROM_Lock();
}

uint32_t getMemory(uint32_t addr)
{
  return *(__IO uint32_t*)addr;
}

/*void initMemory()
{
  SLAVE_ID=getMemory(ADDR_SLAVE_ID);
  MASTER_ID=getMemory(ADDR_MASTER_ID);
  INT_TR=getMemory(ADDR_INT_TR);
  
}*/