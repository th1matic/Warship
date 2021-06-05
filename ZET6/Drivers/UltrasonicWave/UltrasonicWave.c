/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  : UltrasonicWave.c
 * 描述    ：超声波测距模块，UltrasonicWave_Configuration（）函数
             初始化超声模块，UltrasonicWave_StartMeasure（）函数
			 启动测距，并将测得的数据通过串口1打印出来         
 * 实验平台：野火STM32开发板
 * 硬件连接：------------------
 *          | PC8  - TRIG      |
 *          | PC9  - ECHO      |
 *           ------------------
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
*********************************************************************************/
#include "UltrasonicWave.h"
#include "tim.h"

#define	TRIG_PORT      WAVE_TRIG_GPIO_Port		//TRIG       
#define	ECHO_PORT      WAVE_ECHO_GPIO_Port		//ECHO 
#define	TRIG_PIN       WAVE_TRIG_Pin   //TRIG       
#define	ECHO_PIN       WAVE_ECHO_Pin	//ECHO   

double  UltrasonicWave_Distance;      //计算出的距离    

/*
 * 函数名：DelayTime_us
 * 描述  ：1us延时函数
 * 输入  ：Time   	延时的时间 US
 * 输出  ：无	
 */
void DelayTime_us(int Time)    
{
   unsigned char i;
   for ( ; Time>0; Time--)
     for ( i = 0; i < 72; i++ );
}

/*
 * 函数名：UltrasonicWave_CalculateTime
 * 描述  ：计算距离
 * 输入  ：无
 * 输出  ：无	
 */

  


/*
 * 函数名：UltrasonicWave_StartMeasure
 * 描述  ：开始测距，发送一个>10us的脉冲，然后测量返回的高电平时间
 * 输入  ：无
 * 输出  ：无	
 */
void UltrasonicWave_StartMeasure(void)
{
  HAL_GPIO_WritePin(TRIG_PORT,TRIG_PIN,1); 		  //送>10US的高电平
  DelayTime_us(10);		                      //延时20US
  HAL_GPIO_WritePin(TRIG_PORT,TRIG_PIN,0);
  
  while(!HAL_GPIO_ReadPin(ECHO_PORT,ECHO_PIN));	             //等待高电平
  HAL_TIM_Base_Start(&htim5);                                             //开启时钟
  while(HAL_GPIO_ReadPin(ECHO_PORT,ECHO_PIN));	                 //等待低电平

  UltrasonicWave_Distance= __HAL_TIM_GET_COUNTER(&htim5)*0.17;	//所用时间=晶振/分频*计数器  距离=时间*声速（340m/s）	Counter/100k* 34000/2=Counter/ 
	 HAL_TIM_Base_Stop(&htim5);			                                 //定时器2失能
  __HAL_TIM_SET_COUNTER(&htim5,0);
	  
}

	void UltrasonicWave_printf(void)
	{		
	 printf("\r\n测得距离为：%.1f cm\r\n",UltrasonicWave_Distance);
		
	}
/************************END OF FILE************/
