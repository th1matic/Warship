/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  : UltrasonicWave.c
 * ����    �����������ģ�飬UltrasonicWave_Configuration��������
             ��ʼ������ģ�飬UltrasonicWave_StartMeasure��������
			 ������࣬������õ�����ͨ������1��ӡ����         
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ�------------------
 *          | PC8  - TRIG      |
 *          | PC9  - ECHO      |
 *           ------------------
 * ��汾  ��ST3.5.0
 *
 * ����    ��wildfire team 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
*********************************************************************************/
#include "UltrasonicWave.h"
#include "tim.h"

#define	TRIG_PORT      WAVE_TRIG_GPIO_Port		//TRIG       
#define	ECHO_PORT      WAVE_ECHO_GPIO_Port		//ECHO 
#define	TRIG_PIN       WAVE_TRIG_Pin   //TRIG       
#define	ECHO_PIN       WAVE_ECHO_Pin	//ECHO   

double  UltrasonicWave_Distance;      //������ľ���    

/*
 * ��������DelayTime_us
 * ����  ��1us��ʱ����
 * ����  ��Time   	��ʱ��ʱ�� US
 * ���  ����	
 */
void DelayTime_us(int Time)    
{
   unsigned char i;
   for ( ; Time>0; Time--)
     for ( i = 0; i < 72; i++ );
}

/*
 * ��������UltrasonicWave_CalculateTime
 * ����  ���������
 * ����  ����
 * ���  ����	
 */

  


/*
 * ��������UltrasonicWave_StartMeasure
 * ����  ����ʼ��࣬����һ��>10us�����壬Ȼ��������صĸߵ�ƽʱ��
 * ����  ����
 * ���  ����	
 */
void UltrasonicWave_StartMeasure(void)
{
  HAL_GPIO_WritePin(TRIG_PORT,TRIG_PIN,1); 		  //��>10US�ĸߵ�ƽ
  DelayTime_us(10);		                      //��ʱ20US
  HAL_GPIO_WritePin(TRIG_PORT,TRIG_PIN,0);
  
  while(!HAL_GPIO_ReadPin(ECHO_PORT,ECHO_PIN));	             //�ȴ��ߵ�ƽ
  HAL_TIM_Base_Start(&htim5);                                             //����ʱ��
  while(HAL_GPIO_ReadPin(ECHO_PORT,ECHO_PIN));	                 //�ȴ��͵�ƽ

  UltrasonicWave_Distance= __HAL_TIM_GET_COUNTER(&htim5)*0.17;	//����ʱ��=����/��Ƶ*������  ����=ʱ��*���٣�340m/s��	Counter/100k* 34000/2=Counter/ 
	 HAL_TIM_Base_Stop(&htim5);			                                 //��ʱ��2ʧ��
  __HAL_TIM_SET_COUNTER(&htim5,0);
	  
}

	void UltrasonicWave_printf(void)
	{		
	 printf("\r\n��þ���Ϊ��%.1f cm\r\n",UltrasonicWave_Distance);
		
	}
/************************END OF FILE************/
