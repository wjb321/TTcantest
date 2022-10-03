/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
    {
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
    {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
//void SVC_Handler(void)
//{
//}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

#include "ttcan.h"
uint16_t CANSM_ID;
uint16_t CanSMFlag;
uint16_t SOS_ISR_flag = 0;
uint8_t SOS_ISR_Count = 0 ;
extern uint16_t  Received_mes_id[TotNumBC][NumSlot];
extern uint16_t  NumBC;
uint8_t Rx0_DATA0,Rx0_DATA1,Rx0_DATA2,Rx0_DATA3,Rx0_DATA4,Rx0_DATA5,Rx0_DATA6,Rx0_DATA7;
uint16_t CanRefFlag;
uint16_t timeStamp, TimeStampRx1;
//extern uint8_t CAN_DATA0,CAN_DATA1,CAN_DATA2,CAN_DATA3,CAN_DATA4,CAN_DATA5,CAN_DATA6,CAN_DATA7;
void USB_LP_CAN1_RX0_IRQHandler(void)
{

  CanRxMsg RxMessage0;
  CAN_Receive(CAN1,CAN_FIFO0, &RxMessage0);  /* �˺��������ͷ���������˵�,�ڷǱ�Ҫʱ,����Ҫ�Լ��ͷ� */
  CANSM_ID=RxMessage0.StdId;//ExtId;

  Rx0_DATA0=RxMessage0.Data[0]; //matrix row
  Rx0_DATA1=RxMessage0.Data[1]; //matrix column
  Rx0_DATA2=RxMessage0.Data[2]; //low bits of ID
  Rx0_DATA3=RxMessage0.Data[3]; //high bits of ID
  Rx0_DATA4=RxMessage0.Data[4]; // checking if exclusive window?
  Rx0_DATA5=RxMessage0.Data[5];
  TimeStampRx1 = RxMessage0.TimeStampRx;
  //timeStamp += RxMessage0.TimeStampRx;
  //RxMessage0.TimeStampRx =0;
  CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);  /* ��������ж� */
  if(CANSM_ID ==MaskID_SM)
    {
      CanSMFlag = ENABLE;
    }

  else if (CANSM_ID == MaskID_Ref)
    {
      CanRefFlag = ENABLE;
    }
}


//
uint8_t Rx1_DATA0,Rx1_DATA1,Rx1_DATA2,Rx1_DATA3,Rx1_DATA4,Rx1_DATA5,Rx1_DATA6,Rx1_DATA7;
uint8_t CanRxFlag;
uint16_t CANRx_ID;

void CAN1_RX1_IRQHandler(void)
{

  CanRxMsg RxMessage1;
  CAN_Receive(CAN1,CAN_FIFO1, &RxMessage1);  
  CANRx_ID =RxMessage1.StdId;
  Rx1_DATA0=RxMessage1.Data[0];
  Rx1_DATA1=RxMessage1.Data[1]; 
  //Rx1_DATA2=RxMessage1.Data[2];
  Rx1_DATA3=RxMessage1.Data[3]; 
  //Rx1_DATA4=RxMessage1.Data[4]; 
  //Rx1_DATA5=RxMessage1.Data[5];
  //  Rx_DATA6=RxMessage.Data[6];
  //  Rx_DATA7=RxMessage.Data[7];
  CAN_ClearITPendingBit(CAN1,CAN_IT_FMP1);  /* ��������ж� */
  CanRxFlag = ENABLE;

}

#include "can_config.h"
#include "led.h"



//extern uint8_t MasterNumBC;
//uint16_t mes_send_flag = 0;
//uint8_t Transmit_time_flag = 0;

//vu32 NumHighFreq; // for high frequency method

//void TIM2_IRQHandler(void)   //TIM3�ж�
//{
//  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
//    {
//      TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־

//      if(SlaveNode2Flag == 1 )
//        { 
//          NumHighFreq ++;
//         }
//      else if(MasterNodeFlag == 1)  // node2 will count the input A and B phase.
//        {
//          mes_send_flag = ENABLE;
//          MasterNumBC ++;
//					//MasterNodeFlag =0;
//        }

//    }
//}



/*M method*/
//#include "timer.h"

/**
  * @}
  */


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
