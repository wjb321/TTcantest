#include <stdlib.h>
#include <stdio.h>
#include "nodes.h"
#include "can_config.h"
#include "ttcan.h"
#include "stm32f10x.h"
#include "timer.h"
#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "math.h"
#include "led.h"
#include "stm32f10x_it.h"
#include "Speed.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ttcan.h"

#define PI 3.14159
#define CNT_INIT 0

int sm_id_nonzero_flag = 0;
//uint8_t CAN_DATA0,CAN_DATA1,CAN_DATA2,CAN_DATA3,CAN_DATA4,CAN_DATA5,CAN_DATA6,CAN_DATA7;
uint8_t MES1_DATA0,MES1_DATA1,MES1_DATA2,MES1_DATA3,MES1_DATA4,MES1_DATA5,MES1_DATA6,MES1_DATA7;
uint8_t MES2_DATA0,MES2_DATA1,MES2_DATA2,MES2_DATA3,MES2_DATA4,MES2_DATA5,MES2_DATA6,MES2_DATA7;
uint8_t MES3_DATA0,MES3_DATA1,MES3_DATA2,MES3_DATA3,MES3_DATA4,MES3_DATA5,MES3_DATA6,MES3_DATA7;
uint8_t MES4_DATA0,MES4_DATA1,MES4_DATA2,MES4_DATA3,MES4_DATA4,MES4_DATA5,MES4_DATA6,MES4_DATA7;
uint8_t MES5_DATA0,MES5_DATA1,MES5_DATA2,MES5_DATA3,MES5_DATA4,MES5_DATA5,MES5_DATA6,MES5_DATA7;
uint8_t MES6_DATA0,MES6_DATA1,MES6_DATA2,MES6_DATA3,MES6_DATA4,MES6_DATA5,MES6_DATA6,MES6_DATA7;

uint8_t MasterNodeFlag = 0;
uint16_t temp_receive_id, first_mes_after_ref= 0;
uint16_t temp_MesTranTime = 0, temp_sos_ID, interrupt_sos_times = 1, temp= 10000;
uint8_t   SlaveNode1Flag = 0;
uint8_t   SlaveNode2Flag = 0;
/*when receiving the hall speed, get 3 velocity: v1, v2, v3, and compare the diferences*/
int MesTimesInBC = 0 ;
float speedArray[2] = {0};
int VeloVar1 = 0, VeloVar2 = 0 ;
// 填满速度矩阵
/*common parts for node1 and node2*/
float DecelerateRate = 1.5;
float VeloThreshold = 15;
float Phase1 = 0;
float Phase2 = 0;
float Decelerate = 50;
float Radian = 0.523;
float Radius = 0.045;
float intermediate = 0;
/*node 0*/

/*node 1*/

/*node 2*/
/* Tmethod configuration */
int TarrValue = 199; //9 , 19, 29, 39, 49, 59, 69, 79  59
int TpscValue = 71;
int Pulse = 62;
float TMethodSpeed =0.0;
extern int EncoderFlag;
extern int Mmethodflag;
void state_test(int arr, int psc);
/*Mmethod configuration */
int MarrValue = 499;
int MpscValue = 7199;
float MMethodSpeed =0;
extern TaskHandle_t TTCANTask_Handler;

float miniPeriod = 0.0;

int threshold = 1; // for delayUntil(), consider the limitation, add threshold in order to give some more reaction time

void Node0()
{
  CAN__Master_Configuration();
  printf("\r\n");
  printf("*****************************************************************\r\n");
  printf("*                                                               *\r\n");
  printf("*  Master Node！^_^  *\r\n");
  printf("*                                                               *\r\n");
  printf("*****************************************************************\r\n");
  miniPeriod = find_BC_inPSA();
  /*SM sending time*/
  /*BC sending time*/             // 5000 50ms  -> 500 5ms -> 5 0.05ms
  TIM2_Int_Init(miniPeriod-1,71,DISABLE) ;  // miniPeriod -1   arr_sm is for sm sending, BC is for Basic cycle sending
	printf("the miniperiod is: %.3f", miniPeriod/1000.0);
  TIM3_Int_Init(1999,7199,ENABLE);// 4999 for sm time, 14999 for BC time
  MasterNodeFlag = 1;

  /* configuration for vTaskDelayUntil()*/
  TickType_t  PreviousWakeTime;
  const TickType_t TimeIncrement = pdMS_TO_TICKS(miniPeriod/1000.0);
  PreviousWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  while (1)
    {
      vTaskDelayUntil(&PreviousWakeTime, TimeIncrement); // this gurantee the system has a fixed time
      if (SM_ID_Sent_flag == ENABLE)
        {
          SendMatrix(MaskID_SM);
          printf("reference message is sent: %d \r\n", SM_ID_Sent_Counter);
          SM_ID_Sent_flag = DISABLE;
        }

      if(mes_send_flag == ENABLE)
        {
          printf("master %d\n", MasterNumBC);
          SendRef(MaskID_Ref, TotNumBC, NumSlot, SlotTime);
					MasterNumBC++;
          if(MasterNumBC >= TotNumBC)
            {
              MasterNumBC = 0;
            }
					 mes_send_flag = DISABLE;

        }
    }
}





void Node1()
{
  SlaveNode1Flag = 1;
//  int TempDataReceived = 1;
  CANRx_ID = mes4_ID;
//  uint8_t CanRxFlag1  = 1;
  NumBC = 1;
  CAN_Configuration();
  printf("\r\n");
  printf("*****************************************************************\r\n");
  printf("*                                                               *\r\n");
  printf("*  node 1 gets the reference message");
  printf("*                                                               *\r\n");
  printf("*****************************************************************\r\n");
  TIM4_PWM_Init(899,0);	 //不分频。PWM频率=72000000/900=80Khz
  TIM3_Int_Init(3999,7199,DISABLE);//here TIM3 is disabled, so the 3499 has no influence
  intermediate = Decelerate * Radian * Radius;

  TickType_t  PreviousWakeTime;
  const TickType_t TimeIncrement = pdMS_TO_TICKS(15); //threshold
  PreviousWakeTime = xTaskGetTickCount();
		float arb_start_send = 0; 
	arb_start_send = arb_start_sending();
//	UBaseType_t StackUsageMinSize;
//  StackUsageMinSize = uxTaskGetStackHighWaterMark( TTCANTask_Handler );
  while (1)
    {
//			printf("stack 1 min size is: %d\r\n",StackUsageMinSize  );
      vTaskDelayUntil(&PreviousWakeTime, TimeIncrement); // this gurantee the system has a fixed time
//      if(EncoderFlag ==1)
//        {
//          state_test(TarrValue, TpscValue);
//          EncoderFlag =0;

//        }

      /*Reiceive SM from master node*/
      if( CanSMFlag == ENABLE )
        {
          LEDC13 = !LEDC13;
          ReiceiveSM();
          /*add system matrix exclusive and arbitrition window flag
          in the following 2 lines function*/
          build_exclusive_arbitration_window(&received_is_exclusive_window[0][0], TotNumBC, NumSlot);
          last_column_is_arbitration(received_is_exclusive_window, TotNumBC, NumSlot);

          printf("full matrix is: \r\n ");
          for (int i = 0; i < TotNumBC; i++)
            {
              for (int j = 0; j <TotNumBC; j++)
                printf("%d ", received_is_exclusive_window[i][j]);

              printf("\n");///=
            }
          NumBC =0;
          CanSMFlag = DISABLE;
        }

      /*Reiceive reference and start BC*/
      if( CanRefFlag == ENABLE )  // TEST24 CanRefFlag1
        {

          //TIM_Cmd(TIM1, ENABLE); // when basic cycle comes, start counting when to send arbition window messages
          LEDC14 = !LEDC14;
          if(NumBC >= TotNumBC)
            {
              NumBC = 0;
            }
//          printf("\r\n");
//          printf("************ %d. reference start********** \r\n",NumBC);
//          printf("**receive_(data0,data1,data2,data3,data4,data5) = (%d, %d, %d, %d, %d, %d )** \r\n",Rx0_DATA0,Rx0_DATA1,Rx0_DATA2,Rx0_DATA3,Rx0_DATA4,Rx0_DATA5);
//          printf("***time stamp (accumulateed, new) are (%d, %d)*** \r\n",timeStamp, TimeStampRx1);
//          printf("***time stamp (accumulateed, new) in 0x are (%#x, %#x)*** \r\n",timeStamp, TimeStampRx1);
//          printf("************ %d. reference ends********** \r\n",NumBC);
          MesTimesInBC = TriggerTimValAndTimes();
          TimerSlotSet();	// print timer array is:...

          /*get first slave node column IDs*/
//          temp_receive_id= Received_mes_id[NumBC][1];//NumBC
//							printf("temp_receive_id: %x", temp_receive_id);

          /*get get relative execution time*/
//          first_mes_after_ref = MesTransmitTime(temp_receive_id);
//							printf("first_mes_after_ref: %d", first_mes_after_ref);


          //printf("first_mes_after_ref is %d \r\n", first_mes_after_ref);
          //T3 for counting the time slots
//          for(int mestimes = 0; mestimes < MesTimesInBC; mestimes ++)
//            {
//              printf("finalTimerValue[%d] is %d \r\n", mestimes, finalTimerValue[mestimes]);
//            }
          printf("MesTimesInBC is %d \r\n", MesTimesInBC);
          if(MesTimesInBC !=0)
            {
              if(finalTimerValue[0]== 0)
                {
                  printf("++++++++NumBc: %d first message is sent:++++++++++ \r\n", NumBC);
                  //printf("messages should be sent directly after BC comes \r\n");
                  NodeMesTransmit(Received_mes_id[NumBC][1]);
                  //TIM3_Int_Init(finalTimerValue[finalTimerArrayShift_zero],7199,ENABLE); // this gets from timerset()
                  if(MesTimesInBC > 1)
                    {
                      TIM3_Int_Init(finalTimerValue[finalTimerArrayShift_zero] -1,7199,ENABLE);  // this gets from timerset()
                    }
//                  printf("finalTimerValue[finalTimerArrayShift_zero] value: %d++++++++++ \r\n", finalTimerValue[finalTimerArrayShift_zero]);
//                  printf("++++++++Column =1:for message finish++++++++++ \r\n");
                }
              else
                {
                  TIM3_Int_Init(finalTimerValue[0]-1,7199,ENABLE); // this gets from timerset()
                }
            }

          //TEST24 commit the else states
//          else
//            {
//              printf("+++GGG nothing to send ! in NumBc:%d  GGG+++\r\n", NumBC);
//            }
          //NodeMesTransmit(Received_mes_id[NumBC][1]);

          interrupt_sos_times =1;
          NumBC++;

          if(NumBC > TotNumBC)   //3>= 3 // -1  //0
            {
              NumBC = 0; // -1;//
            }
//          printf("************reference end ********** \r\n");
          // printf(" \r\n");
          CanRefFlag = DISABLE;
          finalTimerArrayShift_Nonzero = 1;
          finalTimerArrayShift_zero =1;
          TimerArrayLocation = 0;
        }




      /*SOS triggered internal messages*/
      if(SOS_ISR_flag == ENABLE )
        {

          LEDA0 = !LEDA0;
          printf("##########interrupt %d ########### \r\n",interrupt_sos_times);
          temp_sos_ID = SOS_ID();
          //printf("SOS_ID()=: %#x \r\n", temp_sos_ID);
          printf("##########end interrupt########### \r\n");
          printf(" \r\n");
          temp_MesTranTime = MesTransmitTime(temp_sos_ID);
          // printf("Exact MesTranTime=: %d \r\n", temp_MesTranTime);
          interrupt_sos_times ++ ;
          SOS_ISR_flag = DISABLE;
        }



      if(CanRxFlag  == ENABLE )  //CanRxFlag1
        {
//          u16 led0pwmval=0;
//          u8 dir=1;
          LEDA1 = !LEDA1;
          if(CANRx_ID == mes1_ID)
            {
              LEDB5 = !LEDB5; // 120
              printf("**Mes1 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes2_ID)
            {
              //LEDB6 = !LEDB6; //121
              printf("**Mes2 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes3_ID)
            {
              //LEDB7 = !LEDB7; //122
              printf("**Mes3 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes4_ID)
            {

              printf("**ABS INFO %#x ,%d , %d rps, %#x\r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1,  Rx1_DATA3);
              //printf("%d\r\n", Rx1_DATA1);
              //TEST24 change from  Rx1_DATA0 == 0 below
              while(Rx1_DATA0 ==1)
                {
                  LEDA2 = !LEDA2;// pwm signal
//                  delay_ms(1);
//                  if(dir)led0pwmval = led0pwmval+ 10 ;
//                  //else led0pwmval = led0pwmval- 10;

//                  if(led0pwmval>8)Rx1_DATA0 =1 ; //dir=0;
//                  if(led0pwmval==0)dir=1;
//                  TIM_SetCompare2(TIM4,led0pwmval); // releasing the brake pad
//                  //printf("pwm %d \r\n", led0pwmval);
                }
            }
          else if(CANRx_ID == mes5_ID)
            {
              LEDB9 = !LEDB9; //124
              printf("**Mes5 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes6_ID)
            {
              // LEDA2 = !LEDA2; //125
              printf("**Mes6 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else
            {
              printf("**arbitrary windows **");
            }

          CanRxFlag = DISABLE;
        }
    }

}




void Node2()
{
//1.7.22 down
//  int CountingEncoderNumFlag1 =1;
//  int encoderValue;
  //1.7.22 up
//  int FillArrayTimes = 0;
//  int Veloarray = 0;
  SlaveNode2Flag = 1;
  CAN_Configuration();
  printf("\r\n");
  printf("*****************************************************************\r\n");
  printf("*                                                               *\r\n");
  printf("*  node 2 gets the reference message");
  printf("*                                                               *\r\n");
  printf("*****************************************************************\r\n");
  //TIM1_Int_Init(TarrValue,TpscValue,ENABLE); // T method or M method, 05.07.22 try M method
  TIM2_Int_Init(TarrValue,TpscValue,ENABLE); // T method or M method, 05.07.22 try M method
  TIM3_Int_Init(3499,7199,DISABLE);//10Khz的计数频率，计数到5000为500ms, 9999: 1 s
  TIM4_EncoderMode_Config(Pulse);

//TIM4_PWM_Init(899,0);	 //不分频。PWM频率=72000000/900=80Khz
  intermediate = Decelerate * Radian * Radius;

  TickType_t  PreviousWakeTime;
  const TickType_t TimeIncrement = pdMS_TO_TICKS(15); //threshold
  PreviousWakeTime = xTaskGetTickCount();

  /* get minmal left stack for the current task, the smaller the closer to overflow*/
  UBaseType_t StackUsageMinSize;
  StackUsageMinSize = uxTaskGetStackHighWaterMark( TTCANTask_Handler );
	float arb_start_send = 0; 
	arb_start_send = arb_start_sending();
  while (1)
    {
      /*print minal left stack for current stack, better only uses for test not in the real application*/
      //	printf("stack 1 min size is: %d\r\n",StackUsageMinSize  );
      //vTaskDelay(50);
//			 printf("* 2 node 2 gets the reference message\r\n");
     vTaskDelayUntil(&PreviousWakeTime, TimeIncrement); // this gurantee the system has a fixed time
      if(EncoderFlag ==1)
        {
          state_test(TarrValue, TpscValue);
          EncoderFlag =0; //Tmethod flag

        }

      /*Reiceive SM from master node 1.7.22 commented*/
      if( CanSMFlag == ENABLE ) //
        {
          LEDC13 = !LEDC13;
          CanSMFlag = DISABLE;
          ReiceiveSM();

          /*add system matrix exclusive and arbitrition window flag
          in the following 2 lines function*/
          //build_exclusive_arbitration_window(&received_is_exclusive_window[0][0], TotNumBC, NumSlot);
          //last_column_is_arbitration(received_is_exclusive_window, TotNumBC, NumSlot);
          NumBC =0;
        }

      /*Reiceive reference and start BC*/

      if( CanRefFlag == ENABLE) //CanRefFlag == ENABLE
        {
					TIM1_Int_Init(arb_start_send -1 , 71, ENABLE); //Arb_Win_pscValue
          //TIM_Cmd(TIM1, ENABLE); // when basic cycle comes, start counting when to send arbition window messages
          //LEDC14 = !LEDC14;
          if(NumBC >= TotNumBC)
            {
              NumBC = 0;
            }
          //printf("\r\n");
          //printf("** %d. reference start ** \r\n",NumBC);
          //printf("**receive_(data0,data1,data2,data3,data4,data5) = (%d, %d, %d, %d, %d, %d )** \r\n",Rx0_DATA0,Rx0_DATA1,Rx0_DATA2,Rx0_DATA3,Rx0_DATA4,Rx0_DATA5);
          //printf("***time stamp (accumulateed, new) are (%d, %d)*** \r\n",timeStamp, TimeStampRx1);
          //printf("***time stamp (accumulateed, new) in 0x are (%#x, %#x)*** \r\n",timeStamp, TimeStampRx1);
          //printf("************ %d. reference start********** \r\n",NumBC);
          MesTimesInBC = TriggerTimValAndTimes();

          TimerSlotSet();	// print timer array is:...

          /*get first slave node column IDs*/
//          temp_receive_id= Received_mes_id[NumBC][1];//NumBC
//							printf("temp_receive_id: %x", temp_receive_id);

          /*get get relative execution time*/
//          first_mes_after_ref = MesTransmitTime(temp_receive_id);
//							printf("first_mes_after_ref: %d", first_mes_after_ref);


          //T3 for counting the time slots
//          for(int mestimes = 0; mestimes < MesTimesInBC; mestimes ++)
//            {
//              printf("finalTimerValue[%d] is %d \r\n", mestimes, finalTimerValue[mestimes]);
//            }

//          printf("MesTimesInBC is %d \r\n", MesTimesInBC);
          if(MesTimesInBC !=0)
            {
              if(finalTimerValue[0]== 0)
                {
                  //printf("++++++++NumBc: %d first message is sent:++++++++++ \r\n", NumBC);
                  //printf("messages should be sent directly after BC comes \r\n");
                  NodeMesTransmit(Received_mes_id[NumBC][1]);
                  if(MesTimesInBC > 1)
                    {
                      TIM3_Int_Init(finalTimerValue[finalTimerArrayShift_zero] -1,7199,ENABLE);  // this gets from timerset()
                    }
                  //TIM3_Int_Init(finalTimerValue[finalTimerArrayShift_zero],7199,ENABLE); // this gets from timerset()
                  // printf("finalTimerValue[finalTimerArrayShift_zero] value: %d++++++++++ \r\n", finalTimerValue[finalTimerArrayShift_zero]);
                  // printf("++++++++Column =1:for message finish++++++++++ \r\n");
                }
              else
                {
                  TIM3_Int_Init(finalTimerValue[0] -1,7199,ENABLE);  // this gets from timerset()
                }
            }
          else
            {
              printf("+++GGG nothing to send ! in NumBc:%d  GGG+++\r\n", NumBC);
            }
          interrupt_sos_times =1;
          NumBC++;

          if(NumBC > TotNumBC)   //3>= 3 // -1  //0
            {
              NumBC = 0; // -1;//
            }
          // printf("************reference end ********** \r\n");
          // printf(" \r\n");
          CanRefFlag = DISABLE;
          finalTimerArrayShift_Nonzero = 1;
          finalTimerArrayShift_zero =1;
          TimerArrayLocation = 0;
          //CanRefFlag1 = 0;
        }

      /*SOS triggered internal messages*/
      if(SOS_ISR_flag == ENABLE )  //SOS_ISR_flag == ENABLE
        {

          LEDA0 = !LEDA0;
          printf("##########interrupt %d ########### \r\n",interrupt_sos_times);
          temp_sos_ID = SOS_ID();
          printf("##########end interrupt########### \r\n");
          printf(" \r\n");
          temp_MesTranTime = MesTransmitTime(temp_sos_ID);
          // printf("Exact MesTranTime=: %d \r\n", temp_MesTranTime);
          interrupt_sos_times ++ ;
          SOS_ISR_flag = DISABLE;
        }

      if(CanRxFlag == ENABLE )  //CanRxFlag == ENABLE
        {
//          u16 led0pwmval=0;
//          u8 dir=1;
          //LEDA1 = !LEDA1;
          if(CANRx_ID == mes1_ID)
            {
              LEDB5 = !LEDB5; // 120
              printf("**Mes1 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes2_ID)
            {
              // LEDB6 = !LEDB6; //121
              printf("**Mes2 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes3_ID)
            {
              //LEDB7 = !LEDB7; //122
              printf("**Mes3 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes4_ID)
            {
              LEDA2 = !LEDA2;
              printf("**ABS INFO：(CANRx_ID, wheel status, speed) = ( %#x ,%d , %d rps, %#x)** \r\n", CANRx_ID, Rx1_DATA0, (Rx1_DATA1),  Rx1_DATA3);
              while(Rx1_DATA0 == 1)
                {
                  LEDA2 = !LEDA2;
//                  delay_ms(1);
//                  if(dir)led0pwmval = led0pwmval+ 10 ;
//                  //else led0pwmval = led0pwmval- 10;

//                  if(led0pwmval>1500)dir=0;
//                  if(led0pwmval==0)dir=1;
//                  TIM_SetCompare2(TIM4,led0pwmval); // releasing the brake pad
//                  printf("pwm works now %d \r\n", led0pwmval);
                }
            }
          else if(CANRx_ID == mes5_ID)
            {
              LEDB9 = !LEDB9; //124
              printf("**Mes5 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes6_ID)
            {
              //LEDA2 = !LEDA2; //125
              printf("**Mes6 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else
            {
              printf("**arbitrary windows **");
            }

          CanRxFlag = DISABLE;
        }

      //whileTest =0; //06.07.22 close this test
    }
}


//int Veloarray = 0;
//int FillArrayTimes = 0;
//void state_test(int arr, int psc)
//{

//	      /*Tmethod*/
//          TMethodSpeed = TSpeed(arr, psc);

//          /*Mmethod*/
//          //MMethodSpeed = Mspeed(MarrValue, MpscValue); // try in flag or iqr directly
//          //speedArray[Veloarray] = TMethodSpeed;
//          //printf("TMethodSpeed is this %.3f \r\n", TSpeed(TarrValue, TpscValue) );
//          if(FillArrayTimes < 2)
//            {
//              speedArray[Veloarray] =  2* PI * Radius * TMethodSpeed; //MMethodSpeed
//              if(speedArray[1] + 0.15 < ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 ) && speedArray[1] < speedArray[0] - 0.15 )
//                {
//                  printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
//                  MES4_DATA0 = 1;

//                  //printf("%d \r\n", CAN_DATA0);
//                  printf("it is blocked \r\n");
//                }
//              else if(speedArray[1] + 0.15 >= ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 ) && speedArray[1] < speedArray[0] -0.15 )
//                {
//                  printf("speedArray[1]  is %f\r\n", speedArray[1]);
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
//                  MES4_DATA0 = 2;
//                  //  printf("%d \r\n", CAN_DATA0);
//                  printf("decelerate \r\n");
//                }
//              else if (speedArray[1] >= speedArray[0] + 0.05)
//                {
//                  printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
//                  MES4_DATA0 = 3;
//                  // printf("%d \r\n", CAN_DATA0);
//                  printf("celerate \r\n");
//                }
//              else if (speedArray[0] - 0.09 <=speedArray[1] <= speedArray[0] + 0.09) //||  (VeloVar1 >0 && VeloVar2 == 0)
//                {
////                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
////                      printf("speedArray[0] is %f \r\n", speedArray[0]);
////                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
////											printf("sqrt  is %f \r\n", sqrt(pow(speedArray[0],2) - 4* intermediate));
////											printf("sqrt *2  is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate)) );
//                  printf("constant speed 111\r\n");
//                  MES4_DATA0 = 4;
//                  // printf("%d \r\n", CAN_DATA0);
//                }
//              else
//                {
//                  printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0);
//                  MES4_DATA0 = 5;
//                  // printf("%d \r\n", CAN_DATA0);
//                  printf("other status\r\n");
//                }
//              FillArrayTimes ++;
//              Veloarray ++;  //1  2 3
//            }
//          else
//            {
//              speedArray[0] = speedArray[1] ;
//              speedArray[1] = 2* PI * Radius * TMethodSpeed;
//              //Veloarray ++;
//              FillArrayTimes = 3;
//              if(speedArray[1] + 0.15 < ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 ) && speedArray[1] < speedArray[0] - 0.15 )
//                {
//                  printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
//                  MES4_DATA0 = 1;

//                  //printf("%d \r\n", CAN_DATA0);
//                  printf("it is blocked \r\n");
//                }
//              else if(speedArray[1] + 0.15 >= ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 ) && speedArray[1] < speedArray[0] -0.15 )
//                {
//                  printf("speedArray[1]  is %f\r\n", speedArray[1]);
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
//                  MES4_DATA0 = 2;
//                  //  printf("%d \r\n", CAN_DATA0);
//                  printf("decelerate \r\n");
//                }
//              else if (speedArray[1] >= speedArray[0] + 0.1)
//                {
//                  printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
//                  MES4_DATA0 = 3;
//                  // printf("%d \r\n", CAN_DATA0);
//                  printf("celerate \r\n");
//                }
//              else if (speedArray[0] - 0.09 <=speedArray[1] <= speedArray[0] + 0.09) //||  (VeloVar1 >0 && VeloVar2 == 0)
//                {
////                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
////                      printf("speedArray[0] is %f \r\n", speedArray[0]);
////                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
////											printf("sqrt  is %f \r\n", sqrt(pow(speedArray[0],2) - 4* intermediate));
////											printf("sqrt *2  is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate)) );
//                  printf("constant speed 111\r\n");
//                  MES4_DATA0 = 4;
//                  // printf("%d \r\n", CAN_DATA0);
//                }
//              else
//                {
//                  printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0);
//                  MES4_DATA0 = 5;
//                  // printf("%d \r\n", CAN_DATA0);
//                  printf("other status\r\n");
//                }

//            }
//          MES4_DATA1 = 2* PI * Radius * TMethodSpeed;
//}



