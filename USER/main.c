#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


#include "stm32f10x.h"
#include "usart.h"
#include <stdio.h>
#include "can_config.h"
#include "led.h"
#include "nodes.h"
//#include "trcConfig.h"
//#include "trcDefines.h"
#include "trcRecorder.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sys.h"

//#include <stdlib.h>
////#include "systick.h"
//#include "timer.h"
//#include "ttcan.h"
#define   MasterNode 0
#define   SlaveNode1 1
#define   SlaveNode2 2


#define START_TASK_PRIO		1 // timer confige with highest priority, and stack depth with minimal prio, so other task priority in (1,max-1) range
#define START_STK_SIZE 		150  //148
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);


#define TASK1_TASK_PRIO		4
#define TASK1_STK_SIZE 200
TaskHandle_t TTCANTask_Handler;
void TTCAN_task(void *pvParameters);


#define TASK2_TASK_PRIO		3
#define TASK2_STK_SIZE 		80
TaskHandle_t Task2Task_Handler;
void  Task2(void *pvParameters);


#define TASK3_TASK_PRIO		2
#define TASK3_STK_SIZE 		80  // 120 words = 240 bytes
TaskHandle_t Task3Task_Handler;
//char bufferinfo[1000];
void Task3(void *pvParameters);
//int Arb_Win_arrValue = 199; //9 , 19, 29, 39, 49, 59, 69, 79  59
int Arb_Win_pscValue = 71;
int main(void)
{ 
	float arb_start_send = 0; 
  delay_init();
  LED_Init();
  uart_init(115200);
	arb_start_send = arb_start_sending();
	//TIM4_Int_Init(99,7199);
  //TIM1_Int_Init(arb_start_send -1 , Arb_Win_pscValue, DISABLE);
	TIM1_Int_Init(99 , 7199, ENABLE);
	//SystemClock_Config();
  //USART_Configuration();
 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 	
	vTraceEnable(TRC_START);	
  //create the task, according to the parameters 
  xTaskCreate((TaskFunction_t )start_task,            //task function
                (const char*    )"start_task",          //task name
                (uint16_t       )START_STK_SIZE,        //stack size defin
                (void*          )NULL,                  //parameters for task function
                (UBaseType_t    )START_TASK_PRIO,       //task priority define
                (TaskHandle_t*  )&StartTask_Handler);   //define task handler              
    vTaskStartScheduler();   
							
}

void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����TASK1����
    xTaskCreate((TaskFunction_t )TTCAN_task,             
                (const char*    )"TTCAN_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&TTCANTask_Handler);   
    //����TASK2����
    xTaskCreate((TaskFunction_t )Task2,     
                (const char*    )"Led1_task",   
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler); 
								
		xTaskCreate((TaskFunction_t )Task3,     
                (const char*    )"Led2_task_in_ttcan",   
                (uint16_t       )TASK3_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK3_TASK_PRIO,
                (TaskHandle_t*  )&Task3Task_Handler); 
								
		vTaskDelete(StartTask_Handler); //delete the task: delete the task handler, if it delete itself, can also use NULL
		//vTaskDelete(NULL); 
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//task1������
void TTCAN_task(void *pvParameters)
{
  switch(SlaveNode1) //MasterNode SlaveNode1
    {
    case 0:
      Node0();
      break;
    case 1:
      Node1();
      break;
    case 2:
      Node2();
      break;
    default:
      break;
    }
}

//task2������
void Task2(void *pvParameters)
{ 
	
  u8 task2_num=0;
	UBaseType_t StackUsageMinSize;
  //vTaskList(bufferinfo);
	//printf("bufferinfo is: %s \r\n ", bufferinfo);
  StackUsageMinSize = uxTaskGetStackHighWaterMark( Task2Task_Handler );
	while(1)
	{
//	task2_num++;	//����2ִ�д�����1 ע��task1_num2�ӵ�255��ʱ������㣡��
//		printf("stackminisize_onetask = %d\r\n",StackMiniSize ); // 70 - 60 = 10 then 51*4 = 204 ����TTCAN�������������
//     printf("Teststate 1"); //this is zero, cause the value is enum type, so 0 means running
//		printf("task 1 has executed %d times\r\n",task2_num);
   // LEDA4=!LEDA4;
		LEDA3 = !LEDA3;
		//printf("stack 2 min size is: %d\r\n",StackUsageMinSize  );
    vTaskDelay(1000);                           //��ʱ1s��Ҳ����1000��ʱ�ӽ���	
	}
}

//task2������
void Task3(void *pvParameters)
{
	u8 task1_num=0;
   //UBaseType_t StackUsageMinSize;
  //vTaskList(bufferinfo);
	//printf("bufferinfo is: %s \r\n ", bufferinfo);
  //StackUsageMinSize = uxTaskGetStackHighWaterMark( Task3Task_Handler );
	
	while(1)
	{
		//task1_num++;	//����2ִ�д�����1 ע��task1_num2�ӵ�255��ʱ������㣡��
	//	printf("stack 3 min size is: %d\r\n",StackUsageMinSize  );
	//	printf("Teststate 2"); //this is zero, cause the value is enum type, so 0 means running
//	printf("task 2 has executed %d times\r\n",task1_num);
	  LEDA4 = !LEDA4;
    vTaskDelay(500);                           //��ʱ1s��Ҳ����1000��ʱ�ӽ���	
	}
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  Nonerelavant mes location:
  * @retval None
  */
//PUTCHAR_PROTOTYPE
//{
//  /* Place your implementation of fputc here */
//  /* e.g. write a character to the USART */
//  USART_SendData(USART1, (uint8_t) ch);

//  /* Loop until the end of transmission */
//  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
//    {}

//  return ch;
//}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
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

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

//#include "led.h"
//#include "delay.h"
//#include "key.h"
//#include "usart.h"
//#include "sys.h"
//#include "usart.h"
//#include "timer.h"

// 
///************************************************
// ALIENTEK��ӢSTM32������ʵ��8
// ��ʱ���ж�ʵ��  
// ����֧�֣�www.openedv.com
// �Ա����̣�http://eboard.taobao.com 
// ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
// ������������ӿƼ����޹�˾  
// ���ߣ�����ԭ�� @ALIENTEK
//************************************************/
//	
// int main(void)
// {		
// 
//	delay_init();	    	 //��ʱ������ʼ��	  
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
//	//uart_init();	 //���ڳ�ʼ��Ϊ115200
//	 USART_Configuration();
// 	LED_Init();			     //LED�˿ڳ�ʼ��
//	TIM4_Int_Init(499,7199);//4999 7199: 500ms  10Khz�ļ���Ƶ�ʣ�������5000Ϊ5ms   500: 0.5ms = 500us  50: 0.05ms = 50 us 5: 0.005ms = 5us
//  while(1)
//	{
//		LED0=!LED0;
//		delay_ms(200);		   
//	}	 

// 
//}	 
// 

