#include "timer.h"
#include "usart.h"
#include "nodes.h"
#include "math.h"

/*电机转一圈总的脉冲数(定时器能读到的脉冲数) = 编码器物理脉冲数*编码器倍频*电机减速比 */
#define ENCODER_TIM_PSC  0          /*计数器分频*/
#define ENCODER_TIM_PERIOD  65535   /*计数器最大值*/
#define PI 3.14159
#define CNT_INIT 0  
#define ENCODER_RESOLUTION 11   /*编码器一圈的物理脉冲数*/
#define ENCODER_MULTIPLE 4       /*编码器倍频，通过定时器的编码器模式设置*/
#define MOTOR_REDUCTION_RATIO 4.4 /*电机的减速比*/
#define TOTAL_RESOLUTION    ( ENCODER_RESOLUTION*ENCODER_MULTIPLE*MOTOR_REDUCTION_RATIO ) 
//extern int sampleTime; 

int NumHighFreq =0;
int rounds = 0;
// 读取定时器计数值
 int read_encoder(void) //static
{
    int encoderNum = 0;
	  //delay_ms(1);
    encoderNum = (int)((int16_t)(TIM4->CNT)); /*CNT为uint32, 转为int16*/
    TIM_SetCounter(TIM4, CNT_INIT);/*CNT设初值*/
//	  printf("encoder %d \r\n",encoderNum);
    return encoderNum;
}

//计算电机转速（被另一个定时器每100ms调用1次） 
int calc_motor_rotate_speed() 
{
    int encoderNum = 0;
    float rotateSpeed = 0;
    
    /*读取编码器的值，正负代表旋转方向*/
     encoderNum = read_encoder();
    /* 转速(1秒钟转多少圈)=单位时间内的计数值/总分辨率*时间系数 */
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

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
	
	//定时器TIM1初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //使能指定的TIM1中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;//TIM1_IRQn;  //TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
  TIM_Cmd(TIM1, timer_enable_flag);  //使能TIMx					 
}

void TIM2_Int_Init(int arr,int psc, int timer_enable_flag)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure1;
  NVIC_InitTypeDef NVIC_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

  //定时器TIM2初始化
  TIM_TimeBaseStructure1.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
  TIM_TimeBaseStructure1.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
  TIM_TimeBaseStructure1.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
  TIM_TimeBaseStructure1.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure1); //根据指定的参数初始化TIMx的时间基数单位

  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

  //中断优先级NVIC设置
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
  NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


  TIM_Cmd(TIM2, timer_enable_flag );  //使能TIMx
}


void TIM3_Int_Init(int arr,int psc, int timer_enable_flag)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

  //定时器TIM3初始化
  TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
  TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

  //中断优先级NVIC设置
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
  NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
  TIM_Cmd(TIM3, timer_enable_flag );  //使能TIMx
}

volatile unsigned long long FreeRTOSRunTimeTicks;

//初始化TIM3使其为FreeRTOS的时间统计提供时基
void ConfigureTimeForRunTimeStats(void)
{
	//定时器3初始化，定时器时钟为72M，分频系数为72-1，所以定时器3的频率
	//为72M/72=1M，自动重装载为50-1，那么定时器周期就是50us
	FreeRTOSRunTimeTicks=0;
        //TIM4_Int_Init(50-1,72-1);	//初始化TIM3
}


void TIM4_Int_Init(int arr,int psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
  TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设
							 
}

// encoder mode for Timer4
void TIM4_EncoderMode_Config(int Period)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;   
		NVIC_InitTypeDef NVIC_InitStructure;	

    //PB6 ch1  A,PB7 ch2 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//使能TIM4时钟  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOB时钟

    GPIO_StructInit(&GPIO_InitStructure);//将GPIO_InitStruct中的参数按缺省值输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA6 PA7浮空输入  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;// GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);                           

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = Period;//1495;//7;//7;  //设定计数器重装值   TIMx_ARR = 1024*4-1 这是360线的    1979
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //TIM4时钟预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;//设置时钟分割 T_dts = T_ck_int    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数 
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);              

    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_BothEdge ,TIM_ICPolarity_BothEdge);//使用编码器模式3，上升下降都计数
    TIM_ICStructInit(&TIM_ICInitStructure);//将结构体中的内容缺省输入
    TIM_ICInitStructure.TIM_ICFilter =0xf;// 6;// 0xf//选择输入比较滤波器 
		TIM_SetCounter(TIM4,0); //TIM4->CNT=0
    TIM_Cmd(TIM4, ENABLE); 
    TIM_ICInit(TIM4, &TIM_ICInitStructure);//将TIM_ICInitStructure中的指定参数初始化TIM3

    //溢出中断设置
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //允许TIM4溢出中断
    NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; 
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM4_PWM_Init(int arr,int psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟 //   | RCC_APB2Periph_AFIO
	
	//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM4, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5    
 
   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2

	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
 
	TIM_Cmd(TIM4, ENABLE);  //使能TIM3

}


//定时器3中断服务程序
void TIM1_UP_IRQHandler(void)   //TIM1中断
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //检查TIM1更新中断发生与否
		{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //清除TIMx更新中断标志 
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

void TIM2_IRQHandler(void)   //TIM3中断
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
    {
      TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志

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
void TIM3_IRQHandler(void)   //TIM3中断
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
    {
      TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx更新中断标志
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
void TIM4_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{   
      printf("hello time4\r\\n");		
      LEDE5 = !LEDE5 ;			
			//int number =0;
			//TIM_SetCounter(TIM4, CNT_INIT);/*CNT设初值*/
			//TSpeed(TarrValue, TpscValue );
			//TspeedVal = TSpeed( TarrValue, TpscValue );
		  //FreeRTOSRunTimeTicks++;
			EncoderFlag =1;  //call speed function
	    //number = read_encoder();
		}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update );  //清除TIMx的中断待处理位:TIM 中断源 
}