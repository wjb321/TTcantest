
																		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
																		%      1. !!! means caution     %
																		%      2. ??? means doubts      %
																		%      3. $$$ means solved      
																		%      4. &&& means ideas
																		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Problems:

do the loopback test, there are two different nodes A, B.
  1) try these nodes with this code, A generates the data is what I want while B has different data in CAN_DATA6 and
    CAN_DATA7 parts, when there are CAN_DATA0-7

    对于节点的问题：

    1） 当节点接受到ref小时时候，就会BC ++;
    因为作为基本周期的标志。但是对于具体消息的发送应该考虑是从BC=0 开始进行的。
        所以应该进行BC-1，来进行BC0的操作。但是为了防止BC=0 时候，会产生BC-	1 = -1的情况.就将其调整到0.

            2)  当时想的是如此，使用两个定时器：T3 和T2。
    T3： 用于产生窗口，即在每个既定的窗口开始处进行中断并初始化， 根据接收到的系统矩阵，得到不同的窗口大小（因为不同的消息发送时间不同）。
    T2： 想着消息的发送中的transmission time 代表发送的时间，那么用T2来进行对应系统矩阵的ID进行给定时间的持续发送。即在T3到达中断时候，对T3 和T2 同时进行初始化。
    T2 开始的时候就是对应列表中的ID的发送，当中断时候，对应的ID就停止发送。但是经过考虑是不对的。因为每个对应的窗口只需要发送一次给定的window下的ID值，transmission
    time 代表的是消息在发送后在CAN总线上的持续时间。换言之不需要T２的参与，因为持续的时间是有发送的消息本身决定的。T２可以代表某个消息的“持续时间”，即可以不完全占满
    给定的window，当然最多占完整个window，但是这里出现一个问题：如果T２相比T３在给定的window下太短，那么T２有可能发生两次及两次以上的中断，那么可能会将对应的ID发送多次，
    这并不满足对应消息的周期特性。

    所以应该是这种方案：

    T３定时器，在得到ｒｅｆ之后，就开始初始化定时器，定时器的时间根据不同列的slot长度进行调整：即在每次ＩＳＲ的时候进行一次初始化，那么换言之：通过这种方式就得到对应的exclusive　window。
    那么在对应的初始化时候，就对相应的消息进行发送。然后T３中断到来（本时间窗口截止），进行新的T３窗口初始化，此时又进行对应窗口的ID的发送，直到最后的消息发送完毕。



    3.31.22

Achieved:
    1)
master:
    send system matrix
slave:
    receive the matrix

    2)
master:
    send the reference message with the according to the SM cycle
slave:
    when receive the ref, its BC++, initialize its internal timer immediately(synchronized) and
    when timer isr comes check (BC_counter, SOS_counter) in received SM, and send the message according to the IDs

    3)
master:
    anytime when initializing it
slave:
    restart from BC0 again no matter previous BC comes.

    4)

slave:
    has same time slot(need to be changed to different time slots)

    1.4.22

slave:
    checking the psa list and sm_BC0, triggered at each sos(initialize the timers differently), and make sure which message belongs to which node is going to send


    4.4.22

slave:
    only T3 triggred when BC comes and SOS comes, according to the receiced SM, the message will be sent in the given slot. and now there is no receive node yet.
    Tobias mentioned due to all the nodes receives the SM, and the time is synchronized, so all the nodes know when it sends its message, so it only triggered in the its own
    message slot, there is no need for the SOS interrugp always

      8.4.22

slave:
      T3 can rceive SM, and according its node message list, checking the location of the messsages in the SM, and store the location and time.
	  
	
13.04.22 

master: send matrix and BC as the synchronize signal.
slave: receive the matrix and according to the SM and its own messages list, check the the location of its own IDs in the SM, then send the required IDs according the time.
       the timer will be set according to the location. for example, the timer list is [3000, 8000, 7000], and messages location is [a, 0 ,b],then the timer should be set[0, 8000],
       and in different situations, send the respective messages
	   
01.05.22

ST link uploading code, sometime does not work, should go to this steps: -> options for Target -> debug setting -> debug -> delete the check mark of shearable of ST-linker

02.05. 22

ttcan.c file has configurations for slaveNode1 and slaveNode2, when do the updating and delete the brace by mistake and added in a wrong place. find the bug for a long time.
but it is not convinent when change to different node, i have to come to ttcan.c to configure again. so better way is using a interface then can change to any node.

04.05.22 

a.
solve the bug: when there is only one message after ref like:
slave node2 : 0x01 0x123 0x122 0x121 0 in all the BC, so there is no need to enable timer. it is only enable when finalTimerValue >1

b. 
when changing the matrix: ->> ttcan.h  ->> configure the NumSlot ->> change the matrix in ttcan.c:  mes_id and  is_exclusive_window arrays ->> main.c
switch MasterNode, uploading, switch SlaveNode1/2 uploading.

c.
be careful when uploading to different nodes, switch should be changed repestively!!!

d.
create sm with only one exclusive window.and the values are always same. do the test, the BC timer set 10ms can make sure the all the values cam be safely
sent. if it is faster than 11 ms, some BCs, message can not be sent. checke node0 -> timer2: configure with 110.

f. 
by setting one byte++, setting timer with 170, 7999, there is no data missing. so it should be 17 ms, then the frequency can be 60hz, means 60 samples per second. still need think
how to increase the frequency.

06.05.22
think about  MesTimesInBC = TimerISR(); part how to update, first tempMesLocation is an array to store the nodes messages location in receiced_ID, this part is fixed, so do not need to call 
everytime when BC comes.

8.5.22
try seperate TimerISR into 2 parts, which can save the time. but try the function seems does not work. now first try other parts.

9.5.22
after multiple tests, find the timer for BC can be configured as 16, with 1/ 1.7 = 600HZ, and the messages can be sent and received smoothly.

14.5.22
speed node send speed to ttcan node1(com15) and then send speed info via ttcan to  ttcan node2(3) use the speed to get response

now: ttcan sends speed change info and speed.

15.5.22 
according to different ids, call different functions to send, this different functions are different sensor messages.

16.5.22
pb3, 4 after reset, the main function are jtdo and jntrst modes, when used as noral io, should do the pinremap function.

24.5.22
after checking and debugging, there are some big updates:
1) send speed to node1, and everytime when the bc comes the send the coressponding speed info to node2, only according to the difference of speed, check
it is block or just normal decelerate.

2) the system matrix can be changed randomly, one can send every BC and another one can receice.


10.06.22
USART sends from warship board, and there is a problem:
the speed change in a proper range it regards as deceleration, then remeber: CAN bus can only send data 0-0xff, so maximum is 255, so for m/s is fast enough. but warship board send the speed info is 
1500, which is far higher than the expected value. there are two options: seperate the data into 2 parts, and CAN transmits the data  by data0 and data2. or send relative small value(fair enough)

12.06.22
here add some threshold by checking the speed, and according to the speed info, can fast detect the state of the car.

6.7.22
尝试将Tim2的中断服务函数放在Tim.c中，然后其他的依然保留在原函数中看效果如何。（没变化）
尝试了node2测试单一编码器代码，正常工作。所以应该是V2.0中的代码配置有问题导致其数值不停的变化。
混烧之后，现在v2的代码可以获取编码器的准确值，然后可以稳定地得到对应的速度值， 明天再进行一次测试。
明天： 测试完之后，利用获取的速度值，然后node2 发出速度， 然后node1 接收，工作后，通过将代码只执行一次，再测试其速度，以及time constraint。
the problem is found： all the nodes for counting the frequences should be common GND, otherwise the values will jump.

7.7.22
now encoder mode send rising edges and node1 calculates the speed and then this speed is sent via ttcan and node2 responses according to the state.
but still, the encoder has some jumps when press the keys, but it is pretty normal. probally it needs kalman filter to remove the unexpected jumps.


13.7
ttcan encoder number counting values in basic nodes(14) are different with warship bard(204), probably the timer configures wronly.
checking the difference, ttcan node will be around 18 times slower than the calculation in warship. check it after work

16.7 
warship board uses pwm control motor rotates and encoder values are read by node2 and then the node1 get the car speed states and call pwm to control the motor rotates speed again. here used T method,
while M method can also be executed here still the speed details still need to be specificated(v2.3)

18/8/07
from now on, build freertos on the system.

1. question: .\Obj\Project.axf: Error: L6200E: Symbol PendSV_Handler multiply defined (by port.o and stm32f10x_it.o).
             .\Obj\Project.axf: Error: L6200E: Symbol SVC_Handler multiply defined (by port.o and stm32f10x_it.o).
			 
			 there are conflicts between the isr server function between port.c and it.c: comment these functions out in it.c
			 
21/8/22
1. some api functions are tested
2. main function for tests the task runing time: vTaskGetRunTimeStats();
   a. extern for a variable can be put in the .h file, then any file which include the .h file inside then can use the extern variable.
   b. here uses node0 and  timer 4, priority with preeeption 0 and sub 3, but the tourial has preeption 1 and sub 0, if origin congure does not work, then change to the tourial configure.
   
23/8/22
1. every task can not with empty while loop, it will make the system run fully on the while()
2. some tasks when configuring with some GPIO, it might has different function, like PA0 in warship, it is wk_up, but configure in PA1, then it works
3. https://www.eet-china.com/mp/a63032.html details about concurrency(contains states change diagram)
4. vTaskDelay() and vTaskDelayUntil(), the second one can guarantee the time in the task. also: vApplicationTickHook(), it is the function exactly execute some tasks
5. https://www.cnblogs.com/lizhuming/p/16085130.html     https://blog.csdn.net/wwwlyj123321/article/details/118637117  freertos references

27/8/23
1. add vTaskDelayUntil(), which confirms the time should be fixed in a given time. : in nodes.c -->> node0 : const TickType_t TimeIncrement = pdMS_TO_TICKS(7);
2. in masterNode0: timer3 sends SM to slave node 1, 2 , X, when it finishes sending, TIM3 is closed and TIM2 is opened
3. configurations:stm32f10x_can.c: 
     a. TxMessage->TimeStampTx = (uint16_t)0x0000FFFF & (CANx->sTxMailBox[transmit_mailbox].TDTR >> 16);  // reference in stm32_reference: CAN_RDTxR: P677 
     b. RxMessage->TimeStampRx = (uint16_t)0x0000FFFF & (CANx->sFIFOMailBox[FIFONumber].RDTR >> 16); // reference in stm32_reference: CAN_RDTxR: P680 
	 
	 
29/08/22
1. Hd or md errors: https://blog.csdn.net/zhangxuechao_/article/details/76814698
2. breadboard: 
    a. high led blue :  receive ABS   info: 0x123(node2)
	b. high led green:  block state
	c. low  led green:  receive other info: 0x121(node1)
    d. low  led red  :  send speed_info

31/08/22
1. when using vtaskDelayUntil(); this just make everything inside of the while loop fixed, then TIM2 for highfrequency is fixed, so the Tspeed will keep in constant. so now there is the problem
   the encoder interrupt is fast, and there is going to be a conflict between encoder info and time guaranteen of system matrix.
   faster speed ---> faster encoder interrupt ---> faster speed state updated  ----> faster BC? 

1/9/22
1. stlink: when configuring debug with st mode, then should go to utility to configure with st mode also.
2. stlink cables connection is not ok, has to be pressed when uploading
3. in main function: //task2任务函数 void Other_task(void *pvParameters), there are some functions like:  
     vTaskList(bufferinfo);
	 printf("bufferinfo is: %s \r\n ", bufferinfo);
	 they need quite large cpu space allocation, so here should configure with size 70;
2/9/22
1. after running sometime, rtos for node2 will stop( blinking tasks are stopping working)

9/9/22
1. keil does not support #pragma once
   
wu18946796976 发表于 2019-8-22 08:12
多谢  不支持就没办法了 。。。。

#pragma once

9.90 #pragma once
		Enable the compiler to skip subsequent includes of that header file.
		#pragma once is accepted for compatibility with other compilers, and enables you to use other forms of header guard coding. However, ARM recommends to use #ifndef and #define coding because this is more portable.
		Example
		The following example shows the placement of a #ifndef guard around the body of the file, with a #define of the guard variable after the #ifndef.
		#ifndef FILE_H
		#define FILE_H
		#pragma once         // optional
		... body of the header file ...
		#endif
		The #pragma once is marked as optional in this example. This is because the compiler recognizes the #ifndef header guard coding and skips subsequent includes even if #pragma once is absent.
		
		
12/09/22  
1. in freertosconfig.h file: add, reference is: https://www.youtube.com/watch?v=nw3sEkfbTI0&t=120s
   104 #define configUSE_TRACE_FACILITY        1
   193
   #if (configUSE_TRACE_FACILITY ==1)
   #include "trcRecorder.h"
   #endif
   
2. when using percepio as an analyzer, there are several info:
https://percepio.com/2016/06/16/st-link-rtos-trace-streaming/  (trace analyzer different modes: snapshoot and stream mode)

3. different link debuggers https://www.eet-china.com/mp/a67012.html
4. in file: trcDefines.h 
   #define TRC_RECORDER_MODE_SNAPSHOT		1
   #define TRC_RECORDER_MODE_STREAMING		0
   
5. how to use tracealyzer in keil and stlink: https://zhuanlan.zhihu.com/p/485328857

13/09/22
1. commend this lines in fle trcStreamPort.c, where the stream mode is suppported 
//#include <usb_device.h>
//#include <usbd_CDC_if.h>

2. (freertos snapshot mode)
    https://blog.csdn.net/weixin_43343803/article/details/124779131
3. Low priority numbers denote low priority tasks. The idle task has priority zero (tskIDLE_PRIORITY).
4. https://blog.csdn.net/zhuimeng_ruili/article/details/101308530
   程序的堆栈开的比较小，可能程序在执行一段时间之后，堆栈的使用越来越多，之后就会导致堆栈溢出，该任务死掉 
   解决方法是：在任务调试的阶段使用   uxTaskGetStackHighWaterMark() 来查询任务堆栈的使用情况，如果不够，及时增大堆栈。
5. new firmware from stlink v2 to stlink v2.1 or jlink: https://shequ.stmicroelectronics.cn/thread-617075-1-1.html 
   stlinkreflash.exe for stlink v2 version, this link contains stlinkrefalsh.exe proper version for stlink v2 - jlink OB, not stlink v2.1 to jlink...
   https://mhhauto.com/Thread-Flash-ST-Link-Clone-to-J-link-STlink-Jlink
   
14/09/22
   Integrating the Recorder part
   https://percepio.com/docs/FreeRTOS/manual/index.html#Creating_and_Loading_Traces___Percepio_Trace_Recorder___FreeRTOS___Integrating_the_Recorder
1. Step 10.1 Set TRC_CFG_RECORDER_DATA_INIT to 0 in trcConfig.h. This will prevent RecorderInitialized from being initialized to 0.
2. as the email to percepio tracealyzer support, that I can not see the details of different tasks, is the frequency is too low, but snapshot gives a fixed collect period, then sometimes I can simply collcet the 
   tasks info, sometimes can not. after checking the info, ttcan can confirm the frequency of the tasks, and also can check the execuation time in the node tasks
   
15/09/22
1. the relation between bit, byte and word :http://www.cs.sjsu.edu/~pearce/modules/lectures/co/ds/bits.htm
   1 word = 2 byte = 16 bits. double words = 4 byte = 32 bits
2. forum infomation: https://forums.freertos.org/t/get-error-when-runing-simple-demos/15816/3  

3. for stm32f10ze, the ram is 64 kb , so if #define configTOTAL_HEAP_SIZE	((size_t)(64*1024)) , then definitely that the size for heap is too large, and it caused lots of 
   error: .\Obj\Project.axf: Error: L6406E: No space in execution regions with .ANY selector matching usart.o(.data).
   64* 1024 = 64kb
4. uxTaskGetStackHighWaterMark() 
   this function returns words, in 32bit machine, the return value will be 1 = 4bytes
    * Returns the high water mark of the stack associated with xTask.  That is,
    * the minimum free stack space there has been (in words, so on a 32 bit machine
    * a value of 1 means 4 bytes) since the task started.  
	The smaller the returned number the closer the task has come to overflowing its stack.
	
	add 3 uxTaskGetStackHighWaterMark() for these these current tasks, remember, this is only for test, not for real application.
	
 19/09/22
 1. try many times, tmethod the highNumFreq is always 255 when the speed is very fast, while ui board speed info can be changed.
 try: all node run same code for testing the speed or highNumfrequency, make sure the problem is from the code or from the hardware.
 the code from ui code: UI_thesis
 after testing, when ttcan_freertos guarteen code has some problem which influenced timer value. when mini node and warship node share a same code, mini node only do the encode thing:
 the reaction is same like warship node. so the conclusion is master node is influenced by something, which the minimal timer2 can catch highNumFreuqce is 255.
 question: what make this code tmethod value with some problem???
 result: when using same code, the speed are almost same, which means mini board is ok, the code influce the speed in ttcan.
 
 2. tried quicksort, select one median value in 5 data, but still has some jump. 

20/09.22
1, the speed is always not stable, but the speed can be sent, think a proper way to send the speed, the code supports usart send and receive, here are two walkround:
  a. use usart to receive the relative highnumfreq or just give a fixed highnumfreq
  b. exti way can configure different exti_lines, and when there is different line, the pwm/highNumFreq will be changed directly, just jump out of the encoder part... 
 
21/09/22
1. warship board, when given different speed, node2 highfreqnum changed accordingly, no 255 bound
   mini board, when given different speed, node2 highfreunum won't change or largeest is 255(mini has someissue???)
   for mini board again, change back to encodeflag, and TIM4 encoder only set encodeflag = 1, it works fine,
2. when ttcan task has no delay or untildelay func, it will be occupy all the resource, trace analyzer will only unknown actor,
   so test the execuaction time ect should make sure that the ttcan task has some delays, here delayuntil is working fine.
   
22/09/22
1. after puting the system on, ttcan works properly for around 2 hours and till fine, and only there are some jumps of the speed info. so the jumps are not a urgent issue that has to 
   be solved soon, but the time guarantee stuff
 
********************************************************************************************************************************************************************************
tasks in coming:
a. add in-exclusive in the sm, 
b. give different bc time, and make the system has different time guaranteed
e. bc give time stamp and do the first and second level time sychronization 
c. SM optimization and dynamically updates SM
d. remote frames get ids
f. esp32 nodes update all info to warship lcd
g. lvgl ui design and remote control with freertos build-in, for instance, start the master nodes actions....or contorl all the system and check all the details
h. miniapp info update and pages design
*********************************************************************************************************************************************************************************
24/09/22
1. ??? the ads info not works well always because of the encode some time connections are not good enough
2. $$$ replace usb TTL to cmsis dap link, the downloading is faster, still the speed info has some problem, but it is not the main issue.
3. now start working on task A.
4. !!! checking cmsis dap link blinking led, the usb hub may interfere when 2 slave nodes use one hub: the one stop blinking keeps working while the blinking one stops working.
5. ??? the IDs are given directly, 0x120-0x122 are from node1, 0x123-0x125 are from node2, actually the ids can be produced directly by a function, for example: if the CAN is 
       standard ID, then can give different bits with different type or meanning, use the referenct in Microchip TTCAN: ttcan.c: build_id fuction.
6. ??? arbitration window, how to make the slave nodes realize now it is arbitration window? check if (arbitration_allowed[i] && use_next_arbitration_window[i]) 
7. ??? in arbitration window the can_init() should enable CAN_InitStructure.CAN_NART = DISABLE; /* 报文重传, 如果错误一直传到成功止，否则只传一次 */, in ttcan part disable this function
       in the bachelor reference: slot_used as a flag, then CAN sent, set this flag to one, then it wont send again, but in stm32, can use NART disable method.


25/09/22
1. $$$ when call function,there are some errors, please check x.h, maybe the extern function() has some reference error. like the return format is different.
2. ??? @24/9: 6, arbitration window is built,then what should be done is:
       when exclusive windows finsh sending messages, there are should be some time for arbitration window: t_BC - t_exclusive = t_arbitration,also the configuration of can
3. ??? the mask or list mode, configure that the system can only receive some IDs with a range.
4. ??? how does the remote frame works? should slave nodes store their IDs and master node store all the IDs or only one class node get whole IDs?

26/09/22
1. &&& system matrix can be store in a file, and this system matrix can be generated by genetic algorithm, for example the system matrix can be in the sd card
2. !!! according to psa in the master thesis P28, the message transmission time is around 1Khz ~ 2Khz, timer in stm32 can simply satisfy
3. !!! timer functions in node2:
       TIM4: encoder， when the input encoder number reaches given value then it will causes interrupts, then get the Tspeed
       TIM3: As the timer-triggered timer,which counts the time that allocates to some messages and start next message sending according to the system matrix
       TIM2: Tspeed method/Mspeed method the both need a timer to count the highFreqeucyNum(Tspeed) or Mspeed catch period(Method)
   ??? TIM1: for arbitration window, if the sm is fixed, then ttcan message for each basic cycle is fixed, so here should use one timer to catch the time starts sending arbitration window message, where the slot that messages from all the nodes can be sent same time, the way who sends first judges by the ID priority. the reason why choose TIM1, cause the mini stm32 only has 3 general timer, and they all have some specific usages. this is only for node2,but for node1 or some other nodes in the future, it there still one timer available.
4. ??? for yesterday, creating full sm flag matix example:
									{{1,1,1,0},
									{1,1,1,0},
									{1,1,1,0},
									{1,1,1,0},}	
seems to be not good. cause all the nodes have to notice that the slot = TotalNumSlot -1, and send arbitration message. but the last exclusive window most probably is 2 or 3 different slave nodes sending messages with same period, and they do have to brodcast to all nodes that it is time to send arbitration message, it would make the system more complicated, maybe there are simpler way to do this with the full sm flag matix, let see the future.	   
4. &&& when considering add some messages IDs in the arbitration window, 1, create id generate func, 2, configure the CAN node filter mode, enlarge the range of reception.
5. !!! node2 used to be connected in the usbHUB 2.0 with node1 on 3.0, and node2 stops working after sometimes, but now with only node2
       on usbHUB, it keeps working aroung 1 hour even more. so maybe there should be some conflicts that stop the nodes work when both 
       slave nodes are on the usbHUB.
	   
	   
<<<<<<< HEAD
************************************************************************************************************************************************************
27/09/22
1. !!! dealyUntil(); here add threshold(1ms / 0) for all the nodes when giving time guaranteen
2. !!! int foo(void) : https://stackoverflow.com/questions/42852956/how-to-solve-a-deprecate-declaration-of-a-function-when-compiling-warning
************************************************************************************************************************************************************
28/09/22
1. !!! advanced control timer TIM1, when configuring as a general timer, here should add one TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
       reference:https://blog.csdn.net/Dinvent/article/details/110679546,  http://www.51hei.com/bbs/dpj-27645-1.html
2. !!! try master node in different reaction time: 1ms, 10ms...the BC did not jump: BC: 0,1,2,3..., 
	      while try the BC： 0.1 ms, there is Error:..\FreeRTOS\tasks.c,1146, which makes delayUntil() function does not work properly, cause the function is 
		  in the ms level: 1ms while the function has for BC sending if it is faster than 1ms frequency(0.1ms...) then it will influence delayUntil();
3. !!! when configuring led, make sure GPIO_Mode and GPIO_Speed congifurations are executed before init and setbits and pins
4. ??? timer1 still does not work in ttcan code but works in normal timer interrupt experiment. do believe some configurations are wrong, check it tomorrow.
*************************************************************************************************************************************************************
2/10/22
1. ??? check the update in project: TTCAN_V3.3_timer_not_working read.me, the timer works well, but this project does not work, so need to compare these two
       files differences.
*************************************************************************************************************************************************************
3/10/22
1. !!! wildfile supplies some info about the driving different motors: also PID algorithm:
       https://doc.embedfire.com/motor/motor_tutorial/zh/latest/zero_part/why-study.html