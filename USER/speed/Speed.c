#include "Speed.h"
#include "nodes.h"
#include "math.h"
#include "timer.h"

int Veloarray = 0;
int FillArrayTimes = 0;



void state_test(int arr, int psc)
{         

	      /*Tmethod*/
          TMethodSpeed = TSpeed(arr, psc);

          /*Mmethod*/
          //MMethodSpeed = Mspeed(MarrValue, MpscValue); // try in flag or iqr directly
          //speedArray[Veloarray] = TMethodSpeed;
          //printf("TMethodSpeed is this %.3f \r\n", TSpeed(TarrValue, TpscValue) );
          if(FillArrayTimes < 2)
            {
              speedArray[Veloarray] =  TMethodSpeed; //MMethodSpeed
              if(speedArray[1] + 7 < ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 ) && speedArray[1] < speedArray[0] - 7 )
                {
//                  printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
                  MES4_DATA0 = 1;

                  //printf("%d \r\n", CAN_DATA0);
                  printf("it is blocked \r\n");
                }
              else if(speedArray[1] + 6 >= ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 ) && speedArray[1] < speedArray[0] - 6 )
                {
//                  printf("speedArray[1]  is %f\r\n", speedArray[1]);
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
                  MES4_DATA0 = 2;
                  //  printf("%d \r\n", CAN_DATA0);
                  printf("decelerate \r\n");
                }
              else if (speedArray[1] >= speedArray[0] + 6)
                {
//                  printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
                  MES4_DATA0 = 3;
                  // printf("%d \r\n", CAN_DATA0);
                  printf("celerate \r\n");
                }
              else if (speedArray[0] - 5 <=speedArray[1] <= speedArray[0] + 5) //||  (VeloVar1 >0 && VeloVar2 == 0)
                {
//                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                      printf("speedArray[0] is %f \r\n", speedArray[0]);
//                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
//											printf("sqrt  is %f \r\n", sqrt(pow(speedArray[0],2) - 4* intermediate));
//											printf("sqrt *2  is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate)) );
                  printf("constant speed 111\r\n");
                  MES4_DATA0 = 4;
                  // printf("%d \r\n", CAN_DATA0);
                }
              else
                {
//                  printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0);
                  MES4_DATA0 = 5;
                  // printf("%d \r\n", CAN_DATA0);
                  printf("other status\r\n");
                }
              FillArrayTimes ++;
              Veloarray ++;  //1  2 3
            }
          else
            {
              speedArray[0] = speedArray[1] ;
              speedArray[1] = TMethodSpeed;
              //Veloarray ++;
              FillArrayTimes = 3;
               if(speedArray[1] + 7 < ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 ) && speedArray[1] < speedArray[0] - 7 )
                {
//                  printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
                  MES4_DATA0 = 1;

                  //printf("%d \r\n", CAN_DATA0);
                  printf("it is blocked \r\n");
                }
              else if(speedArray[1] + 6 >= ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 ) && speedArray[1] < speedArray[0] - 6 )
                {
//                  printf("speedArray[1]  is %f\r\n", speedArray[1]);
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
                  MES4_DATA0 = 2;
                  //  printf("%d \r\n", CAN_DATA0);
                  printf("decelerate \r\n");
                }
              else if (speedArray[1] >= speedArray[0] + 6)
                {
//                  printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
                  MES4_DATA0 = 3;
                  // printf("%d \r\n", CAN_DATA0);
                  printf("celerate \r\n");
                }
              else if (speedArray[0] - 5 <=speedArray[1] <= speedArray[0] + 5) //||  (VeloVar1 >0 && VeloVar2 == 0)
                {
//                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                      printf("speedArray[0] is %f \r\n", speedArray[0]);
//                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
//											printf("sqrt  is %f \r\n", sqrt(pow(speedArray[0],2) - 4* intermediate));
//											printf("sqrt *2  is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate)) );
                  printf("constant speed 111\r\n");
                  MES4_DATA0 = 4;
                  // printf("%d \r\n", CAN_DATA0);
                }
              else
                {
//                  printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0);
                  MES4_DATA0 = 5;
                  // printf("%d \r\n", CAN_DATA0);
                  printf("other status\r\n");
                }

            }
          MES4_DATA1 = TMethodSpeed;
}

//          /*Tmethod*/
//          TMethodSpeed = TSpeed(TarrValue, TpscValue);

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
//              else if (speedArray[0] - 0.04 <=speedArray[1] <= speedArray[0] + 0.04) //||  (VeloVar1 >0 && VeloVar2 == 0)
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
//              else if (speedArray[1] >= speedArray[0] + 0.05)
//                {
//                  printf("speedArray[1]  is %f \r\n", speedArray[1] );
//                  printf("speedArray[0] is %f \r\n", speedArray[0]);
//                  printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 );
//                  MES4_DATA0 = 3;
//                  // printf("%d \r\n", CAN_DATA0);
//                  printf("celerate \r\n");
//                }
//              else if (speedArray[0] - 0.04 <=speedArray[1] <= speedArray[0] + 0.04) //||  (VeloVar1 >0 && VeloVar2 == 0)
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
          //Mmethodflag = 0;
					