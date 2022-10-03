#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"


int read_encoder(void); //static
int calc_motor_rotate_speed(void);
float TSpeed(int arr, int psc );
float Mspeed(int arr, int psc);  //psc here is 7199, so it is fine

extern int TarrValue ;
extern int TpscValue;
extern int MarrValue ;
extern int MpscValue;
extern float TMethodSpeed ;
extern float Radius;
extern int Pulse;
//void TIM3_Int_Init(u16 arr,u16 psc);
void TIM1_Int_Init(int arr,int psc, int timer_enable_flag);
void TIM3_Int_Init(int arr,int psc, int timer_enable_flag);
void TIM2_Int_Init(int arr,int psc, int timer_enable_flag);
void TIM4_Int_Init(int arr,int psc); //basic config
void TIM4_PWM_Init(int arr,int psc); //node1
void TIM4_EncoderMode_Config(int Period);
extern volatile unsigned long long FreeRTOSRunTimeTicks;
void ConfigureTimeForRunTimeStats(void);
#endif
