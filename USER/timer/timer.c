#include "timer.h"
#include "usart.h"
#include "nodes.h"
#include "math.h"

/*���תһȦ�ܵ�������(��ʱ���ܶ�����������) = ����������������*��������Ƶ*������ٱ� */
#define ENCODER_TIM_PSC  0          /*��������Ƶ*/
#define ENCODER_TIM_PERIOD  65535   /*���������ֵ*/
#define PI 3.14159
#define CNT_INIT 0  
#define ENCODER_RESOLUTION 11   /*������һȦ������������*/
#define ENCODER_MULTIPLE 4       /*��������Ƶ��ͨ����ʱ���ı�����ģʽ����*/
#define MOTOR_REDUCTION_RATIO 4.4 /*����ļ��ٱ�*/
#define TOTAL_RESOLUTION    ( ENCODER_RESOLUTION*ENCODER_MULTIPLE*MOTOR_REDUCTION_RATIO ) 
//extern int sampleTime; 

int NumHighFreq =0;
int rounds = 0;
// ��ȡ��ʱ������ֵ
 int read_encoder(void) //static
{
    int encoderNum = 0;
	  //delay_ms(1);
    encoderNum = (int)((int16_t)(TIM4->CNT)); /*CNTΪuint32, תΪint16*/
    TIM_SetCounter(TIM4, CNT_INIT);/*CNT���ֵ*/
//	  printf("encoder %d \r\n",encoderNum);
    return encoderNum;
}

//������ת�٣�����һ����ʱ��ÿ100ms����1�Σ� 
int calc_motor_rotate_speed() 
{
    int encoderNum = 0;
    float rotateSpeed = 0;
    
    /*��ȡ��������ֵ������������ת����*/
     encoderNum = read_encoder();
    /* ת��(1����ת����Ȧ)=��λʱ���ڵļ���ֵ/�ֱܷ���*ʱ��ϵ�� */
     rotateSpeed = (float)(encoderNum/(TOTAL_RESOLUTION*0.15));  //0.05 //0.23 
   // printf("encoder %d \r\n",encoderNum);
   // printf("rotateSpeed  %.2f \r\n",rotateSpeed);
	return rotateSpeed;
	
}


float Mspeed(int arr, int psc)  //psc here is 7199, so it is fine
{
	int encoderNum = 0;
  float Mv = 0;
	encoderNum = read_encoder() + 1496* rounds;
	Mv = (60* encoderNum/(TOTAL_RESOLUTION * (arr/pow(10.0,4.0)))) ;  //differences between two nodes the counting value is around 18
	rounds = 0;
	//printf("rounds %d\n",rounds);
	//printf("encoderNum %d\n",encoderNum);
	//printf("%d\n",TIM4->CNT);
	//printf("%.3f\n",(TOTAL_RESOLUTION * (arr/pow(10.0,4.0))));
	printf("Mv is %.3f r/m \n",Mv);
	
	return Mv;
	//printf("%.3f\n",Mv);
}

float TSpeed(int arr, int psc )
{ 
	//static int counter = 0;
	float Tv = 0;
	int freq = 0;
	freq = (int)(1.0/(((arr+1)*(psc +1)) / (72.0* pow(10.0,6.0))));
	printf("T_NumHighFreq is %d r/m \r\n", NumHighFreq);
	Tv = (2.0* PI * Radius * 60* freq * Pulse )/ (TOTAL_RESOLUTION * NumHighFreq *1.0 );  // Tv = 2*pi*n*r   * 2* PI * Radius *
  //printf("freq is %d \r\n", freq);
	//printf("%.3f \r\n", 1.7+ (arr-9)/10.0);
  //printf("NumHighFreq is %d \r\n", NumHighFreq);
	//if(counter >10) counter = 0;
  printf("%.3f \r\n", Tv);
	NumHighFreq = 0;
	return Tv;
	//printf("Tv is %.3f \r\n", Tv);
}

void TIM1_Int_Init(int arr, int psc, int timer_enable_flag)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM1��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM1�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;//TIM1_IRQn;  //TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
  TIM_Cmd(TIM1, timer_enable_flag);  //ʹ��TIMx					 
}

void TIM2_Int_Init(int arr,int psc, int timer_enable_flag)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure1;
  NVIC_InitTypeDef NVIC_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��

  //��ʱ��TIM2��ʼ��
  TIM_TimeBaseStructure1.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
  TIM_TimeBaseStructure1.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
  TIM_TimeBaseStructure1.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
  TIM_TimeBaseStructure1.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure1); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

  //�ж����ȼ�NVIC����
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
  NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


  TIM_Cmd(TIM2, timer_enable_flag );  //ʹ��TIMx
}


void TIM3_Int_Init(int arr,int psc, int timer_enable_flag)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

  //��ʱ��TIM3��ʼ��
  TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
  TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

  //�ж����ȼ�NVIC����
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
  NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
  TIM_Cmd(TIM3, timer_enable_flag );  //ʹ��TIMx
}

volatile unsigned long long FreeRTOSRunTimeTicks;

//��ʼ��TIM3ʹ��ΪFreeRTOS��ʱ��ͳ���ṩʱ��
void ConfigureTimeForRunTimeStats(void)
{
	//��ʱ��3��ʼ������ʱ��ʱ��Ϊ72M����Ƶϵ��Ϊ72-1�����Զ�ʱ��3��Ƶ��
	//Ϊ72M/72=1M���Զ���װ��Ϊ50-1����ô��ʱ�����ھ���50us
	FreeRTOSRunTimeTicks=0;
        //TIM4_Int_Init(50-1,72-1);	//��ʼ��TIM3
}


void TIM4_Int_Init(int arr,int psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
  TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx����
							 
}

// encoder mode for Timer4
void TIM4_EncoderMode_Config(int Period)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;   
		NVIC_InitTypeDef NVIC_InitStructure;	

    //PB6 ch1  A,PB7 ch2 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//ʹ��TIM4ʱ��  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��

    GPIO_StructInit(&GPIO_InitStructure);//��GPIO_InitStruct�еĲ�����ȱʡֵ����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA6 PA7��������  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;// GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);                           

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = Period;//1495;//7;//7;  //�趨��������װֵ   TIMx_ARR = 1024*4-1 ����360�ߵ�    1979
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //TIM4ʱ��Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;//����ʱ�ӷָ� T_dts = T_ck_int    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ��� 
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);              

    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_BothEdge ,TIM_ICPolarity_BothEdge);//ʹ�ñ�����ģʽ3�������½�������
    TIM_ICStructInit(&TIM_ICInitStructure);//���ṹ���е�����ȱʡ����
    TIM_ICInitStructure.TIM_ICFilter =0xf;// 6;// 0xf//ѡ������Ƚ��˲��� 
		TIM_SetCounter(TIM4,0); //TIM4->CNT=0
    TIM_Cmd(TIM4, ENABLE); 
    TIM_ICInit(TIM4, &TIM_ICInitStructure);//��TIM_ICInitStructure�е�ָ��������ʼ��TIM3

    //����ж�����
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //����TIM4����ж�
    NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; 
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM4_PWM_Init(int arr,int psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ�� //   | RCC_APB2Periph_AFIO
	
	//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM4, ENABLE); //Timer3������ӳ��  TIM3_CH2->PB5    
 
   //���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2

	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
 
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM3

}


//��ʱ��3�жϷ������
void TIM1_UP_IRQHandler(void)   //TIM1�ж�
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //���TIM1�����жϷ������
		{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
			LEDE5 = !LEDE5;
			printf("CAN time1 now send arbitration window messages \n");
	  //TIM_Cmd(TIM1, DISABLE); // when basic cycle comes, start counting when to send arbition window messages
		}
}


extern uint8_t MasterNodeFlag ;
extern uint8_t MasterNumBC;
uint16_t mes_send_flag = 0;
uint8_t Transmit_time_flag = 0;
uint16_t Mmethodflag = 0;

void TIM2_IRQHandler(void)   //TIM3�ж�
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
    {
      TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־

      if(SlaveNode2Flag == 1 )
        { 
					//Mspeed(MarrValue, MpscValue);
					//Mmethodflag = 1; //Mmethod for the flag testing
					NumHighFreq ++;	
          //LEDB8 = !LEDB8; 					
         }
      else if(MasterNodeFlag == 1)  // node2 will count the input A and B phase.
        {
          mes_send_flag = ENABLE;
//					 if(MasterNumBC >= TotNumBC)
//            {
//              MasterNumBC = 0;
//            }
          //MasterNumBC ++;
					//MasterNodeFlag =0;
        }

    }
}


extern uint8_t MasterNodeFlag ;
uint32_t SM_ID_Sent_Counter = 0;
uint16_t SM_ID_Sent_flag = 0;
void TIM3_IRQHandler(void)   //TIM3�ж�
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
    {
      TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx�����жϱ�־
      if(MasterNodeFlag == 1)
        {
          SM_ID_Sent_Counter++;
          if (SM_ID_Sent_Counter >= 10)
            {
              printf("Now the SM is sent to all the slave nodes");
              TIM_Cmd(TIM3, DISABLE);
              TIM_Cmd(TIM2, ENABLE);
            }
          SM_ID_Sent_flag = ENABLE;
							//MasterNodeFlag =0;
        }
      else
        {
          SOS_ISR_flag = ENABLE;
        }

    }
}

int EncoderFlag = 0;
float TspeedVal = 0;
void TIM4_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{   
      printf("hello time4\r\\n");		
      LEDE5 = !LEDE5 ;			
			//int number =0;
			//TIM_SetCounter(TIM4, CNT_INIT);/*CNT���ֵ*/
			//TSpeed(TarrValue, TpscValue );
			//TspeedVal = TSpeed( TarrValue, TpscValue );
		  //FreeRTOSRunTimeTicks++;
			EncoderFlag =1;  //call speed function
	    //number = read_encoder();
		}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
}