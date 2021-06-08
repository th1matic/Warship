/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sys.h"
#include "delay.h"
#include "ILI93xx.h"

#include "key.h"
#include "24cxx.h"
#include "touch.h"

#include "GUI.h"

#include "WindowDLG.h"


#include "malloc.h"

#include "PulseSensor.h"
#include "UltrasonicWave.h"
#include "stdio.h"

#include "hal_key.h"
#include "gizwits_product.h"
#include "common.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

#include <string.h>
 
#define RXBUFFERSIZE 256     //最大接收字节数
char RxBuffer[RXBUFFERSIZE];  //接收数据
uint8_t aRxBuffer;			//接收中断缓冲
uint8_t Uart3_Rx_Cnt = 0;		//接收缓冲计数


extern uint8_t bRxBuffer;			//接收中断缓冲



#define GPIO_KEY_NUM 2 ///< Defines the total number of key member
keyTypedef_t singleKey[GPIO_KEY_NUM]; ///< Defines a single key member array pointer
keysTypedef_t keys;  

extern uint8_t bRxBuffer;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void sendDataToProcessing(char symbol, int dat );

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
* key1 short press handle
* @param none
* @return none
*/
void key1ShortPress(void)
{
    GIZWITS_LOG("KEY1 PRESS ,Production Mode\n");
    gizwitsSetMode(WIFI_PRODUCTION_TEST);
}

/**
* key1 long press handle
* @param none
* @return none
*/
void key1LongPress(void)
{
    GIZWITS_LOG("KEY1 PRESS LONG ,Wifi Reset\n");
    gizwitsSetMode(WIFI_RESET_MODE);

}

/**
* key2 short press handle
* @param none
* @return none
*/
void key2ShortPress(void)
{
    GIZWITS_LOG("KEY2 PRESS ,Soft AP mode\n");
    #if !MODULE_TYPE
    gizwitsSetMode(WIFI_SOFTAP_MODE);
    #endif
}

/**
* key2 long press handle
* @param none
* @return none
*/
void key2LongPress(void)
{
    //AirLink mode
    GIZWITS_LOG("KEY2 PRESS LONG ,AirLink mode\n");
    #if !MODULE_TYPE
    gizwitsSetMode(WIFI_AIRLINK_MODE);
    #endif
	 GIZWITS_LOG("2353252\n");
}

/**
* Key init function
* @param none
* @return none
*/
void keyInit(void)
{
    singleKey[0] = keyInitOne(NULL, KEY1_GPIO_Port, KEY1_Pin, key1ShortPress, key1LongPress);
    singleKey[1] = keyInitOne(NULL, KEY2_GPIO_Port, KEY2_Pin, key2ShortPress, key2LongPress);
    keys.singleKey = (keyTypedef_t *)&singleKey;
    keyParaInit(&keys); 
}

//清空屏幕并在右上角显示"RST"
void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//清屏   
 	POINT_COLOR=BLUE;//设置字体为蓝色 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//显示清屏区域
  	POINT_COLOR=RED;//设置画笔蓝色 
}
////////////////////////////////////////////////////////////////////////////////
//电容触摸屏专有部分
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
//两个数之差的绝对值 
//x1,x2：需取差值的两个数
//返回值：|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  
//画一条粗线
//(x1,y1),(x2,y2):线条的起始坐标
//size：线条的粗细程度
//color：线条的颜色
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		gui_fill_circle(uRow,uCol,size,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   
////////////////////////////////////////////////////////////////////////////////
//5个触控点的颜色(电容触摸屏用)												 
const u16 POINT_COLOR_TBL[5]={RED,GREEN,BLUE,BROWN,GRED};  
////电阻触摸屏测试函数
//void rtp_test(void)
//{
//	u8 key;
//	u8 i=0;	  
//	while(1)
//	{
//	 	key=KEY_Scan(0);
//		tp_dev.scan(0); 		 
//		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
//		{	
//		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
//			{	
//				if(tp_dev.x[0]>(lcddev.width-24)&&tp_dev.y[0]<16)Load_Drow_Dialog();//清除
//				else TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],RED);		//画图	  			   
//			}
//		}else delay_ms(10);	//没有按键按下的时候 	    
//		if(key==KEY0_PRES)	//KEY0按下,则执行校准程序
//		{
//			LCD_Clear(WHITE);	//清屏
//		    TP_Adjust();  		//屏幕校准 
//			TP_Save_Adjdata();	 
//			Load_Drow_Dialog();
//		}
//		i++;
//	}
//}
//电容触摸屏测试函数
void ctp_test(void)
{
	u8 t=0;
	u8 i=0;	  	    
 	u16 lastpos[5][2];		//最后一次的数据 
	while(1)
	{
		tp_dev.scan(0);
		for(t=0;t<5;t++)
		{
			if((tp_dev.sta)&(1<<t))
			{
                //printf("X坐标:%d,Y坐标:%d\r\n",tp_dev.x[0],tp_dev.y[0]);
				if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)
				{
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
                    
					lcd_draw_bline(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//画线
					lastpos[t][0]=tp_dev.x[t];
					lastpos[t][1]=tp_dev.y[t];
					if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<20)
					{
						Load_Drow_Dialog();//清除
					}
				}
			}else lastpos[t][0]=0XFFFF;
		}
		
		delay_ms(5);i++;
	}	
}

//触摸屏定位设置
void Mytouch_MainTask(void)
{
    GUI_PID_STATE TouchState;
    int xPhys;
    int yPhys;
    GUI_Init();
    GUI_SetFont(&GUI_Font20_ASCII);
	
   	GUI_CURSOR_Show();/////在这出问题
    GUI_CURSOR_Select(&GUI_CursorCrossL);/////
	
    GUI_SetBkColor(GUI_WHITE);
    GUI_SetColor(GUI_BLACK);
    GUI_Clear();
    GUI_DispString("Measurement of\nA/D converter values");
    while (1)
    {
        GUI_TOUCH_GetState(&TouchState); // Get the touch position in pixel
        xPhys = GUI_TOUCH_GetxPhys(); // Get the A/D mesurement result in x
        yPhys = GUI_TOUCH_GetyPhys(); // Get the A/D mesurement result in y
        GUI_SetColor(GUI_BLUE);
        GUI_DispStringAt("Analog input:\n", 0, 40);
        GUI_GotoY(GUI_GetDispPosY() + 2);
        GUI_DispString("x:");
        GUI_DispDec(xPhys, 4);
        GUI_DispString(", y:");
        GUI_DispDec(yPhys, 4);
        GUI_SetColor(GUI_RED);
        GUI_GotoY(GUI_GetDispPosY() + 4);
        GUI_DispString("\nPosition:\n");
        GUI_GotoY(GUI_GetDispPosY() + 2);
        GUI_DispString("x:");
        GUI_DispDec(TouchState.x,4);
        GUI_DispString(", y:");
        GUI_DispDec(TouchState.y,4);
        delay_ms(50);
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  MX_CRC_Init();
  MX_TIM3_Init();
  MX_TIM6_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM5_Init();
  MX_USART3_UART_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  delay_init(72);               		//初始化延时函数
	TFTLCD_Init();           				//初始化LCD FSMC接口

	tp_dev.init();				   		//触摸屏初始化 
		
		my_mem_init(SRAMIN); 		//初始化内部内存池
	my_mem_init(SRAMEX);  		//初始化外部内存池
//	
		GUI_Init();
		
		
// KEY_Init();
 LED_Init();
	
	HAL_TIM_Base_Start_IT(&htim2);
	
	HAL_TIM_Base_Start_IT(&htim3);
	 
  HAL_TIM_Base_Start_IT(&htim6);
	
	HAL_TIM_Base_Start_IT(&htim4);	
//  HAL_UART_Receive_IT(&huart1,OT_RxBuffer,1);
	
	
//	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);
	HAL_UART_Receive_IT(&huart3, (uint8_t *)&aRxBuffer, 1);
	
//HAL_UART_Receive_IT(&huart2, (uint8_t *)&bRxBuffer, 1);//开启下一次接收中断
//HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);//开启下一次接收中断
 uartInit();



	userInit();
	gizwitsInit();
	keyInit();
	GIZWITS_LOG("MCU Init Success \n");
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//  while (1)
//  {
//		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	
//	GUI_SetBkColor(GUI_BLUE);   //设置背景颜色
//  GUI_SetColor(GUI_YELLOW);   //设置颜色
//  GUI_Clear();                //清屏
//  GUI_SetFont(&GUI_Font24_ASCII); //设置字体
//  GUI_DispStringAt("HELLO WORD!", 0, 0);

// GUI_TOUCH_Exec();
//  Mytouch_MainTask();






CreateWindow();


//		GUI_TOUCH_Exec();
//		GUI_Delay(50); 
//GUI_Exec();


//    GIZWITS_LOG("KEY2 PRESS LONG ,AirLink mode\n");
//    #if !MODULE_TYPE
//    gizwitsSetMode(WIFI_AIRLINK_MODE);
//    #endif
//	 GIZWITS_LOG("2353252\n");

//	    GIZWITS_LOG("KEY2 PRESS ,Soft AP mode\n");
//    #if !MODULE_TYPE
//    gizwitsSetMode(WIFI_SOFTAP_MODE);
//    #endif
//   GIZWITS_LOG("aasdsfafaa");

while (1)
{

	
  GUI_Exec();
	if (beat_flag == 1)
	{
		
		SendDataSBQ();

//			 FUN_ICON0Clicked();

//		  printf("666\n");
	}
	
	else if (height_flag == 1)
	{
							UltrasonicWave_StartMeasure();

	  UltrasonicWave_printf();
	}
	

//	  else if (wifi_flag == 1)
//  {

	 userHandle();
//				printf ("well");
		gizwitsHandle((dataPoint_t *)&currentDataPoint);
//  }
	
}
  
	
//	}

//	 /*触摸屏测试函数*/
//  	POINT_COLOR=RED;
//	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
//	LCD_ShowString(30,70,200,16,16,"TOUCH TEST");	
//	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
//	LCD_ShowString(30,110,200,16,16,"2019/9/19");	 		
//   	if(tp_dev.touchtype!=0XFF)
//	{
//		LCD_ShowString(30,130,200,16,16,"Press KEY0 to Adjust");//电阻屏才显示
//	}
//	delay_ms(1500);
// 	Load_Drow_Dialog();	 	
//	
//	if(tp_dev.touchtype&0X80)ctp_test();//电容屏测试
//	else rtp_test(); 					//电阻屏测试  
//





  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */





void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	

	
	
	if (htim == (&htim2))
//	if(htim->Instance==htim2.Instance)
	{
	PulseSensor_CallBack();	
	}
	
	
	
    else if (htim == (&htim3))
    {
     OS_TimeMS++;
//		
//			if (OS_TimeMS > 1000)
//			{
////				GUI_TOUCH_Exec();
//			
////					GUI_Exec();
//////			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);
//				OS_TimeMS = 0;
//			}
    }
		else if (htim == (&htim6))
		{
		GUI_TOUCH_Exec();

//					GUI_Exec();

    
//			if (OS_TimeMS > 500)
//			{
//					HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);

////				OS_TimeMS = 0;
//			}


		}
		
	else if(htim==&htim4)
	{
//				printf ("well");
			keyHandle((keysTypedef_t *)&keys);
			gizTimerMs();
	}
		
}


//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if (huart == (&huart1))
//	{
//	
//   if (OT_RxBuffer == "1")
//	 {
//		 HAL_UART_Transmit(&huart1, OT_RxBuffer, 1, 1000);
////	 printf("获得数据");
//	 }
// }
//	
//  HAL_UART_Receive_IT(&huart1,OT_RxBuffer,1);
//}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
 
	if (huart == (&huart3))	
{
	
	if(Uart3_Rx_Cnt >= 255)  //溢出判断
	{
		Uart3_Rx_Cnt = 0;
		memset(RxBuffer,0x00,sizeof(RxBuffer));
		HAL_UART_Transmit(&huart1, (uint8_t *)"数据溢出", 10,0xFFFF); 	
        
	}
	else
	{
		RxBuffer[Uart3_Rx_Cnt++] = aRxBuffer;   //接收数据转存
	
		if((RxBuffer[Uart3_Rx_Cnt-1] == 0x0A)&&(RxBuffer[Uart3_Rx_Cnt-2] == 0x0D)) //判断结束位
		{
//			HAL_UART_Transmit(&huart1, (uint8_t *)&RxBuffer, Uart1_Rx_Cnt,0xFFFF); //将收到的信息发送出去			
		
			if (RxBuffer[0] == 0x31)
			{
         printf ("心率指令\r\n");
				RxBuffer[0] = 0x00;
				beat_flag = 1;
			  height_flag = 0;
							FUN_ICON0Clicked();
			}
			
			else if (RxBuffer[0] == 0x32)
			{
         printf ("身高指令\r\n");
				RxBuffer[0] = 0x00;			
			   height_flag = 1;
			   beat_flag = 0;				
							FUN_ICON1Clicked();				
			}
			
			else if (RxBuffer[0] == 0x33)
			{
         printf ("血压指令\r\n");
				RxBuffer[0] = 0x00;				
			}

			else if (RxBuffer[0] == 0x34)
			{
         printf ("体重指令\r\n");
				RxBuffer[0] = 0x00;
			}	
			
			else if (RxBuffer[0] == 0x35)
			{
         printf ("无线指令\r\n");
				RxBuffer[0] = 0x00;
				   wifi_flag = 1;
		      	beat_flag = 0;
			     height_flag = 0;
			}		
			
			while(HAL_UART_GetState(&huart3) == HAL_UART_STATE_BUSY_TX);//检测UART发送结束
			Uart3_Rx_Cnt = 0;
			memset(RxBuffer,0x00,sizeof(RxBuffer)); //清空数组
		
		
		}
	}

	HAL_UART_Receive_IT(&huart3, (uint8_t *)&aRxBuffer, 1);   //再开启接收中断
	}

	
	 if(huart == (&huart2))  
    {  
//				gizPutData((uint8_t *)&bRxBuffer, 1);

//        HAL_UART_Receive_IT(&huart2, (uint8_t *)&bRxBuffer, 1);//开启下一次接收中断  
//   
      gizPutData((uint8_t *)&bRxBuffer, 1);
			HAL_UART_Receive_IT(&huart2, (uint8_t *)&bRxBuffer, 1);//开启下一次接收中断 
	
		}  
	
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
