#ifndef USART_H
#define USART_H
#include "led.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "delay.h" 
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

void uart_init(u32 bound);
void GPIO_Configuration(void);
void USART_Configuration(void);
void NVIC0_Configuration(void);
void NVIC1_Configuration(void);	
#endif
