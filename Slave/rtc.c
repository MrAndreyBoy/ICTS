#include "rtc.h"
#include "memory.h"

RTC_InitTypeDef RTC_InitStruct;
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
RTC_AlarmTypeDef RTC_AlarmStruct;

uint16_t mos[]= {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
uint16_t vmos[]={0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};

void RTC_Config()
{ 
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_RTCAccessCmd(ENABLE);

  /* Check if the StandBy flag is set */
  if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
  {
    /* Clear StandBy flag */
    PWR_ClearFlag(PWR_FLAG_SB);
  
    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();
    /* No need to configure the RTC as the RTC config(clock source, enable,
       prescaler,...) are kept after wake-up from STANDBY */
  }
  else
  {
    setMemory(ADDR_MODE, 0);
    /* RTC Configuration ******************************************************/
    /* Reset RTC Domain */
    RCC_RTCResetCmd(ENABLE);
    RCC_RTCResetCmd(DISABLE);
  
    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);
  
    /* Wait till LSE is ready */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  
    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);
  
    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();
  
    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStruct.RTC_AsynchPrediv = 0x7F;
    RTC_InitStruct.RTC_SynchPrediv = 0xFF;
    RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
  
    RTC_Init(&RTC_InitStruct);
  
    /* Configure the time register */
    //RTC_TimeStruct.RTC_H12     = RTC_H12_AM;
    RTC_TimeStruct.RTC_Hours = 0;
    RTC_TimeStruct.RTC_Minutes = 0;
    RTC_TimeStruct.RTC_Seconds = 0;
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
    
    /*RTC_DateStruct.RTC_Year=70;
    RTC_DateStruct.RTC_Month=1;
    RTC_DateStruct.RTC_Date=1;
    
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);*/
  }
  
  RTC_ClearFlag(RTC_FLAG_ALRAF);
}

void gotoSleep()
{
  uint32_t temp32, temp32_1, temp32_2;
  uint8_t m, h;
  
  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
  
  switch(getMemory(ADDR_MODE))
  {
  case 2:
    {
      temp32=getMemory(ADDR_MSR_TIME);
      temp32_1=getMemory(ADDR_TRM_TIME);
      temp32_2=getMemory(ADDR_CAL_TIME);
      if(temp32_2>getRTCtime())
      {
        if((temp32_2<=temp32)&&(temp32_2<=temp32_1))
        {
          setMemory(ADDR_MODE, 3);
          setAlarmtime(getMemory(ADDR_CAL_TIME));
          break;
        }
      }
      if(temp32<temp32_1) setAlarmtime(getMemory(ADDR_MSR_TIME));
      else setAlarmtime(getMemory(ADDR_TRM_TIME));
      break;
    }
  case 3:
    {
      if(getMemory(ADDR_CAL_NUM)==0)
      {
        if(getMemory(ADDR_CTR_TIME)>getRTCtime())
        {
          setAlarmtime(getMemory(ADDR_CTR_TIME));
          break;
        }
        else
        {
          uint8_t stat_nom = (getMemory(ADDR_TEST_FLAG)?STAT_NOM_T:STAT_NOM);
          uint32_t stat_iom = (getMemory(ADDR_TEST_FLAG)?STAT_IOM_T:STAT_IOM);
          
          setMemory(ADDR_MODE, 2); 
          m=(getMemory(ADDR_MSR_NUM)&0xFF);
          h=(getMemory(ADDR_MSR_NUM)>>8);
          setMemory(ADDR_TEMP_MEM+(m+1)*4+h*(0x80),0xFFFFFFFF);
          setMemory(ADDR_TEMP_MEM+((m+1)%stat_nom+1)*4+((h+(m+1)/stat_nom)%7)*(0x80),0xFFFFFFFF);
          setMemory(ADDR_TEMP_MEM+((m+2)%stat_nom+1)*4+((h+(m+2)/stat_nom)%7)*(0x80),0xFFFFFFFF);
          setMemory(ADDR_TEMP_MEM+((m+3)%stat_nom+1)*4+((h+(m+3)/stat_nom)%7)*(0x80),0xFFFFFFFF);
          temp32=getMemory(ADDR_MSR_TIME);
          if(((m+3)/stat_nom)||(m==0)) setMemory(ADDR_TEMP_MEM+((h+1)%7)*(0x80),temp32+stat_iom*(4-((m+4)%stat_nom)));
          h=(h+(m+4)/stat_nom)%(getMemory(ADDR_TEST_FLAG)?STAT_NOP_T:STAT_NOP);
          m=(m+4)%stat_nom;
          setMemory(ADDR_MSR_NUM, ((h<<8)|m));
          setMemory(ADDR_MSR_TIME,temp32+stat_iom*4);
          temp32=getMemory(ADDR_TRM_TIME);
          if(temp32<getRTCtime())
          {
            setMemory(ADDR_TRM_TIME,temp32+(getMemory(ADDR_TEST_FLAG)?STAT_IOT_T:STAT_IOT));
          }
          temp32=getMemory(ADDR_MSR_TIME);
          temp32_1=getMemory(ADDR_TRM_TIME);
          if(temp32<temp32_1) setAlarmtime(getMemory(ADDR_MSR_TIME));
          else setAlarmtime(getMemory(ADDR_TRM_TIME));
          break;
        }
      }
      else
      {
        temp32=getMemory(ADDR_CAL_TIME);
        temp32_1=getMemory(ADDR_CTR_TIME);
        if(temp32<temp32_1) setAlarmtime(getMemory(ADDR_CAL_TIME));
          else setAlarmtime(getMemory(ADDR_CTR_TIME));
          break;
      }
      break;
    }
  default:
    {
      setAlarmtime(getMemory(ADDR_MSR_TIME));
      break;
    }
  }

  setMemory(ADDR_STOP_FLAG, 0);

  PWR_ClearFlag(PWR_FLAG_WU);
  RTC_ClearFlag(RTC_FLAG_ALRAF);
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
  PWR_EnterSTANDBYMode();
}

void reload()
{
  setAlarmtime(getRTCtime()+5);
  
  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
  PWR_ClearFlag(PWR_FLAG_WU);
  RTC_ClearFlag(RTC_FLAG_ALRAF);
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
  PWR_EnterSTANDBYMode();
}

void setRTCtime(uint32_t time)
{
    uint16_t days,year,i;

    RTC_TimeStruct.RTC_Seconds=time%60;
    RTC_TimeStruct.RTC_Minutes=time%3600/60;
    RTC_TimeStruct.RTC_Hours=time%86400/3600;
    RTC_TimeStruct.RTC_H12=RTC_H12_AM;
    
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
    
    days=time/86400;
    year=((days-365*(days/365))>=(days/365+1)/4) ? (days/365) : (days/365-1);
    RTC_DateStruct.RTC_Year=(year+1970)%100;
    days=days-365*year-(year+2)/4;
    i=11;
    while(days<(MONTHDAY(year+1970,i))) i--;
    RTC_DateStruct.RTC_Month=i+1;
    RTC_DateStruct.RTC_Date=days-MONTHDAY(year+1970,i)+1;
    
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
}

void setRTChour(uint8_t hour)
{
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
  RTC_TimeStruct.RTC_Hours=hour;
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
}

void setRTCmin(uint8_t min)
{
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
  RTC_TimeStruct.RTC_Minutes=min;
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
}

void setRTCsec(uint8_t sec)
{
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
  RTC_TimeStruct.RTC_Seconds=sec;
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
}

void setAlarmtime(uint32_t wakeTime)
{
  uint32_t temp32=wakeTime;
  
  RTC_AlarmStruct.RTC_AlarmTime.RTC_Seconds = temp32%60;
  RTC_AlarmStruct.RTC_AlarmTime.RTC_Minutes = temp32%3600/60;
  RTC_AlarmStruct.RTC_AlarmTime.RTC_Hours = temp32%86400/3600;
  RTC_AlarmStruct.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
  
  RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);
}

uint32_t getRTCtime()
{
  uint32_t time;
  uint32_t year;
  
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
  
  year=(RTC_DateStruct.RTC_Year<50)?(RTC_DateStruct.RTC_Year+2000):(RTC_DateStruct.RTC_Year+1900);
  
  time=RTC_TimeStruct.RTC_Seconds;
  time+=60*RTC_TimeStruct.RTC_Minutes;
  time+=3600*RTC_TimeStruct.RTC_Hours;
  time+=86400*(RTC_DateStruct.RTC_Date-1);
  time+=86400*(MONTHDAY(year,RTC_DateStruct.RTC_Month-1));
  time+=86400*(365*(year-1970)+(year-1969)/4);
  
  return time;
}

uint8_t getRTChour()
{
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
  return RTC_TimeStruct.RTC_Hours;
}

uint8_t getRTCmin()
{
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
  return RTC_TimeStruct.RTC_Minutes;
}

uint8_t getRTCsec()
{
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
  return RTC_TimeStruct.RTC_Seconds;
}