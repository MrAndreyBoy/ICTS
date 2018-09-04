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
#include "tsensor.h"
#include "onewire.h"
#include "uartwire.h"
#include "transceiver.h"
#include "executor.h"
#include "rtc.h"
#include "masterwork.h"
    
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
uint32_t Time1;
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
    
    RTC_Config();
    
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
    
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
    GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_40MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    
    packetlength=0;
    packet_ready=0;
    USART_InitStruct.USART_BaudRate=115200;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStruct);
    USART_Cmd(USART3, ENABLE);
    
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  
    transceiverInit();
    transceiverConfig();
    transceiverManualCalibration();
    transceiverRXOn();
    
    uw_init_term();
    
    slave_num=0;
    
  /* Infinite loop */
    Time=getTime();
    while (1)
    {
      executor();
      if(Delay(Time)>=500)
      {
        transceiverReadPacket_mode();
        GPIO_ToggleBits(GPIOB, GPIO_Pin_1);
        Time=getTime();
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
