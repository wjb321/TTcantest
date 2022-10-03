    
	#include "led.h"
	
	uint8_t Display1;

//	void led_on_off13()
//		{
//			if( Display1 ) 
//	   { 
//        /*====LED-ON=======*/
//	    GPIO_SetBits(GPIOC , GPIO_Pin_13);
//	    }  
//	  else 
//	  { 
//	    /*====LED-OFF=======*/ 
//	    GPIO_ResetBits(GPIOC , GPIO_Pin_13);
//	  }
//	  Display1 = ~Display1;   	
//	  delay_ms(200);  /* delay 200ms */  
//   }
//		
//		uint8_t Display2;

//	void led_on_off14()
//		{
//			if( Display2 ) 
//	   { 
//        /*====LED-ON=======*/
//			GPIO_SetBits(GPIOC , GPIO_Pin_14);
//	    }  
//	  else 
//	  { 
//	    /*====LED-OFF=======*/ 
//			GPIO_ResetBits(GPIOC , GPIO_Pin_14);
//	  }
//	  Display2 = ~Display2;   	
//	  delay_ms(200);  /* delay 200ms */  
//   }
//		
//	 	uint8_t Display3;

//	void led_on_off15()
//		{
//			if( Display3 ) 
//	   { 
//        /*====LED-ON=======*/
//	    GPIO_SetBits(GPIOA , GPIO_Pin_0);
//	    }  
//	  else 
//	  { 
//	    /*====LED-OFF=======*/ 
//	    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
//	  }
//	  Display3 = ~Display3;   	
//	  delay_ms(100);  /* delay 200ms */  
//    }
		

		
	 
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	

 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz	
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;				 //LED0-->PB.5 �˿�����
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 GPIO_ResetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);						 //PB.5 �����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //LED1-->PE.5 �˿�����, �������
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_13); 						 //PE.5 ����� 
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;	    		 //LED1-->PE.5 �˿�����, �������
 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_ResetBits(GPIOB, GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9); 
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //LED1-->PE.5 �˿�����, �������
 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOE,GPIO_Pin_5); 						 //PE.5 ����� 
}
