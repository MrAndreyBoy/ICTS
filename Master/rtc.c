#include "rtc.h"

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
    RTC_TimeStruct.RTC_H12     = RTC_H12_AM;
    RTC_TimeStruct.RTC_Hours = 0;
    RTC_TimeStruct.RTC_Minutes = 0;
    RTC_TimeStruct.RTC_Seconds = 0;
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
    
    RTC_DateStruct.RTC_Date  = 0;
    RTC_DateStruct.RTC_Month = 0;
    RTC_DateStruct.RTC_Year  = 0;
  }
  RTC_ClearFlag(RTC_FLAG_ALRAF);
}

void gotoSleep(uint8_t h, uint8_t m, uint8_t s)
{
  RTC_AlarmStruct.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
  RTC_AlarmStruct.RTC_AlarmTime.RTC_Hours   = h;
  RTC_AlarmStruct.RTC_AlarmTime.RTC_Minutes = m;
  RTC_AlarmStruct.RTC_AlarmTime.RTC_Seconds = s;
  RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);
  
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