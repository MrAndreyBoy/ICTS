/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    16-May-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l1xx_it.h"
#include "uartwire.h"
#include "transceiver.h"
#include "cc112x_spi.h"
#include "rtc.h"
#include "memory.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef TIM_InitStruct;
TIM_OCInitTypeDef  TIM_OCInitStruct;
GPIO_InitTypeDef GPIO_InitStruct;
USART_InitTypeDef USART_InitStruct;
NVIC_InitTypeDef NVIC_InitStruct;
uint32_t Time;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32l1xx_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32l1xx.c file
     */ 
    uint8_t temp;
    uint8_t buffer[128] = {0};
    uint8_t m, h, i;

  /* Add your application code here
     */
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    TIM_InitStruct.TIM_Prescaler = 7;       
    TIM_InitStruct.TIM_Period = 65535;
    TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Timing;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 1000;
    TIM_OC1Init(TIM2, &TIM_OCInitStruct);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
   
    TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
       
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_SetBits(GPIOB, GPIO_Pin_1|GPIO_Pin_2);
    GPIO_ResetBits(GPIOB, GPIO_Pin_1|GPIO_Pin_2);
    
    RTC_Config();
    
    uw_init_term();
    
    switch(getMemory(ADDR_MODE))
    {
    case 0: //hand mode
      {
        transceiverInit();
        transceiverConfig();
        GPIO_SetBits(GPIOB, GPIO_Pin_1);
        cc112xSpiReadReg(CC112X_PARTNUMBER,&temp, 1);
        if(temp!=0x48) while(1){}
        GPIO_SetBits(GPIOB, GPIO_Pin_2);
        transceiverManualCalibration();
        GPIO_ResetBits(GPIOB, GPIO_Pin_1);
        transceiverRXOn();
        Time=getTime();
        while (1)
        {
          if(Delay(Time)>=500)
          {
            GPIO_ToggleBits(GPIOB, GPIO_Pin_1);
            transceiverReadPacket();
            Time=getTime();
          }
        }
        break;
      }
    case 1: //automeasure mode
      {
        uint32_t temp32;
        
        temp32=getMemory(ADDR_MSR_TIME);
        setMemory(ADDR_MSR_TIME,temp32+getMemory(ADDR_MSR_INT)); 
        
        m=(getMemory(ADDR_MSR_NUM)&0xFF);
        h=(getMemory(ADDR_MSR_NUM)>>8);
        
        if(m==0) 
        {
          setMemory(ADDR_TEMP_MEM+h*(0x80),temp32);
          temp32=0;
          for(i=0; i<getMemory(ADDR_MSR_CNT); i++)
            setMemory(ADDR_TEMP_MEM+h*(0x80)+4*(i+1),temp32);
        }
        sleep(1000);
        temp32=uw_get_term()<<3;   //1-Wire
        
        setMemory(ADDR_TEMP_MEM+(m+1)*4+h*(0x80),temp32);
        m=(m+1)%(getMemory(ADDR_MSR_CNT));
        
        if(m==0)
        {
          h=(h+1)%7;
          setMemory(ADDR_MSR_NUM, (h<<8));
          
          transceiverInit();
          transceiverConfig();
          transceiverManualCalibration();
          temp32=getRTCtime();
          buffer[3]=0x24;
          buffer[4]=(uint8_t)(temp32>>24);
          buffer[5]=(uint8_t)(temp32>>16);
          buffer[6]=(uint8_t)(temp32>>8);
          buffer[7]=(uint8_t)temp32;    
          transceiverSendPacket(buffer, 7);
          transceiverRXOn();
          
          Time=getTime();
          m=0;
          while (m<10)
          {
            if(Delay(Time)>=500)
            {
              if(getMemory(ADDR_STOP_FLAG)==0) m++;
              GPIO_ToggleBits(GPIOB, GPIO_Pin_1);
              transceiverReadPacket();
              Time=getTime();
            }
          }
        }
        else
        {
          setMemory(ADDR_MSR_NUM, ((h<<8)|m));
        }
        gotoSleep(0);
        break;
      }
    case 2: //stationary mode
      {
        uint32_t temp32, temp32_1;
        uint8_t test_fl;
        
        test_fl=getMemory(ADDR_TEST_FLAG);
        setMemory(ADDR_MSR_CNT, (test_fl?STAT_NOM_T:STAT_NOM));
        temp32=getMemory(ADDR_MSR_TIME);
        temp32_1=getMemory(ADDR_TRM_TIME);
        
        if(temp32<temp32_1) //making measure
        {
          GPIO_SetBits(GPIOB, GPIO_Pin_1);
          
          temp32=getMemory(ADDR_MSR_TIME);
          setMemory(ADDR_MSR_TIME,temp32+(test_fl?STAT_IOM_T:STAT_IOM));
        
          m=(getMemory(ADDR_MSR_NUM)&0xFF);
          h=(getMemory(ADDR_MSR_NUM)>>8);
        
          if(m==0) 
          {
            setMemory(ADDR_TEMP_MEM+h*(0x80),temp32);
            temp32=0;
            for(i=0; i<getMemory(ADDR_MSR_CNT); i++)
            setMemory(ADDR_TEMP_MEM+h*(0x80)+4*(i+1),temp32);
          }
          temp32=uw_get_term()<<3;   //1-Wire
          setMemory(ADDR_TEMP_MEM+(m+1)*4+h*(0x80),temp32);
          
          m=(m+1)%(test_fl?STAT_NOM_T:STAT_NOM);
          if(m==0)
          {
            h=(h+1)%(test_fl?STAT_NOP_T:STAT_NOP);
          }          
          setMemory(ADDR_MSR_NUM, ((h<<8)|m));
        }
        else
        {
          temp32=getMemory(ADDR_TRM_TIME);
          setMemory(ADDR_TRM_TIME,temp32+(test_fl?STAT_IOT_T:STAT_IOT));
        
          transceiverInit();
          transceiverConfig();
          transceiverManualCalibration();
          temp32=getRTCtime();
          buffer[3]=0x24;
          buffer[4]=(uint8_t)(temp32>>24);
          buffer[5]=(uint8_t)(temp32>>16);
          buffer[6]=(uint8_t)(temp32>>8);
          buffer[7]=(uint8_t)temp32;    
          transceiverSendPacket(buffer, 7);
          transceiverRXOn();
          
          Time=getTime();
          m=0;
          while (m<10)
          {
            if(Delay(Time)>=500)
            {
              if(getMemory(ADDR_STOP_FLAG)==0) m++;
              GPIO_ToggleBits(GPIOB, GPIO_Pin_2);
              transceiverReadPacket();
              Time=getTime();
            }
          }
        }
        gotoSleep();
        break;
      }
    case 3: //calibration mode
      {
        uint32_t temp32, temp32_1;
        uint8_t test_fl;
        
        test_fl=getMemory(ADDR_TEST_FLAG);
        //setMemory(ADDR_MSR_CNT, (test_fl?CALB_NOM_T:CALB_NOM));
        
        temp32=getMemory(ADDR_CAL_TIME);
        temp32_1=getMemory(ADDR_CTR_TIME);
        
        if((temp32<temp32_1)&&(getRTCtime()-temp32<2)) //making measure
        {
          GPIO_SetBits(GPIOB, GPIO_Pin_2);
          
          temp32=getMemory(ADDR_CAL_TIME);
          setMemory(ADDR_CAL_TIME,temp32+(test_fl?CALB_IOM_T:CALB_IOM)); 
        
          m=(getMemory(ADDR_CAL_NUM)&0xFF);
          h=(getMemory(ADDR_CAL_NUM)>>8);
        
          if(m==0) 
          {
            setMemory(ADDR_CAL_MEM+h*(0x80),temp32);
            temp32=0;
            for(i=0; i<CALB_NOM; i++)
            setMemory(ADDR_CAL_MEM+h*(0x80)+4*(i+1),temp32);
          }
          temp32=uw_get_term()<<3;   //1-Wire
          setMemory(ADDR_CAL_MEM+(m+1)*4+h*(0x80),temp32);
          m=(m+1)%(test_fl?CALB_NOM_T:CALB_NOM);
          
          if(m==0)
          {
            h=(h+1)%(test_fl?CALB_NOP_T:CALB_NOP);
          }          
          setMemory(ADDR_CAL_NUM, ((h<<8)|m));
        }
        else
        {
          temp32=getMemory(ADDR_CTR_TIME);
          if(getMemory(ADDR_CAL_NUM)!=0) setMemory(ADDR_CTR_TIME,temp32+(test_fl?CALB_IOT_T:CALB_IOT));
        
          transceiverInit();
          transceiverConfig();
          transceiverManualCalibration();
          temp32=getRTCtime();
          buffer[3]=0x22;
          buffer[4]=(uint8_t)(temp32>>24);
          buffer[5]=(uint8_t)(temp32>>16);
          buffer[6]=(uint8_t)(temp32>>8);
          buffer[7]=(uint8_t)temp32;    
          transceiverSendPacket(buffer, 7);
          transceiverRXOn();
          
          Time=getTime();
          m=0;
          while (m<10)
          {
            if(Delay(Time)>=500)
            {
              if(getMemory(ADDR_STOP_FLAG)==0) m++;
              GPIO_ToggleBits(GPIOB, GPIO_Pin_1);
              transceiverReadPacket();
              Time=getTime();
            }
          }
        }
        gotoSleep();
        break;
      }
    }
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
