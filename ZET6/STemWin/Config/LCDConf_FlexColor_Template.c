/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.44 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf_FlexColor_Template.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license SLA0044,
  * the "License"; You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *                      http://www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "GUI.h"
#include "GUIDRV_FlexColor.h"
//#include "GUIDRV_Template.h"
//#include "touch.h"
#include "ILI93xx.h"

/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/

////
//// Physical display size
////
//#define XSIZE_PHYS  240 // To be adapted to x-screen size
//#define YSIZE_PHYS  320 // To be adapted to y-screen size

//与触摸屏有关定义，根据实际情况填写
#define TOUCH_AD_TOP		160  	//按下触摸屏的顶部，写下 Y 轴模拟输入值。
#define TOUCH_AD_BOTTOM		3990 	//按下触摸屏的底部，写下 Y 轴模拟输入值。
#define TOUCH_AD_LEFT 		160		//按下触摸屏的左侧，写下 X 轴模拟输入值。
#define TOUCH_AD_RIGHT		3990	//按下触摸屏的右侧，写下 X 轴模拟输入值。


//屏幕大小
#define XSIZE_PHYS  320 //X轴
#define YSIZE_PHYS  240 //Y轴
#define VXSIZE_PHYS	320 
#define VYSIZE_PHYS 240


/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   GUICC_565
  #error Color conversion not defined!
#endif
#ifndef   GUIDRV_FLEXCOLOR
  #error No display driver defined!
#endif

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/
/********************************************************************
*
*       LcdWriteReg
*
* Function description:
*   Sets display register
*/
//static void LcdWriteReg(U16 Data) {
//  // ... TBD by user
//}

/********************************************************************
*
*       LcdWriteData
*
* Function description:
*   Writes a value to a display register
*/
//static void LcdWriteData(U16 Data) {
//  // ... TBD by user
//}

/********************************************************************
*
*       LcdWriteDataMultiple
*
* Function description:
*   Writes multiple values to a display register.
*/
//static void LcdWriteDataMultiple(U16 * pData, int NumItems) {
//  while (NumItems--) {
//    // ... TBD by user
//  }
//}

/********************************************************************
*
*       LcdReadDataMultiple
*
* Function description:
*   Reads multiple values from a display register.
*/
//static void LcdReadDataMultiple(U16 * pData, int NumItems) {
//  while (NumItems--) {
//    // ... TBD by user
//  }
//}

/*********************************************************************
*
*       Public functions
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Function description:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
//void LCD_X_Config(void) {
//  GUI_DEVICE * pDevice;
//  CONFIG_FLEXCOLOR Config = {0};
//  GUI_PORT_API PortAPI = {0};
//  //
//  // Set display driver and color conversion
//  //
//  pDevice = GUI_DEVICE_CreateAndLink(GUIDRV_FLEXCOLOR, GUICC_565, 0, 0);
//  //
//  // Display driver configuration, required for Lin-driver
//  //
//  LCD_SetSizeEx (0, XSIZE_PHYS , YSIZE_PHYS);
//  LCD_SetVSizeEx(0, VXSIZE_PHYS, VYSIZE_PHYS);
//  //
//  // Orientation
//  //
//  Config.Orientation = GUI_SWAP_XY | GUI_MIRROR_Y;
//  GUIDRV_FlexColor_Config(pDevice, &Config);
//  //
//  // Set controller and operation mode
//  //
//  PortAPI.pfWrite16_A0  = LcdWriteReg;
//  PortAPI.pfWrite16_A1  = LcdWriteData;
//  PortAPI.pfWriteM16_A1 = LcdWriteDataMultiple;
//  PortAPI.pfReadM16_A1  = LcdReadDataMultiple;
//  GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66708, GUIDRV_FLEXCOLOR_M16C0B16);
//}



//配置程序,用于创建显示驱动器件,设置颜色转换程序和显示尺寸
void LCD_X_Config(void) {
	GUI_DEVICE_CreateAndLink(&GUIDRV_Template_API, GUICC_M565, 0, 0); 
	//配置显示驱动接口与颜色转换格式，链接至指定图层设备。
	LCD_SetSizeEx    (0, lcddev.width, lcddev.height);//设置物理屏幕区域
	LCD_SetVSizeEx   (0, lcddev.width, lcddev.height);//设置虚拟屏幕区域
  if(lcddev.id == 0X5510) //0X5510为4.3寸 800x480的屏
	{
		if(lcddev.dir == 0) //竖屏  // 2001:3520 6352
			{					    // 9147:3832 6288
			GUI_TOUCH_Calibrate(GUI_COORD_X,0,480,0,479);
			GUI_TOUCH_Calibrate(GUI_COORD_Y,0,800,0,799);
		}else //横屏
		{
			//GUI_TOUCH_SetOrientation(GUI_SWAP_XY|GUI_MIRROR_Y); 
			GUI_TOUCH_Calibrate(GUI_COORD_X,0,480,0,479); 		
			GUI_TOUCH_Calibrate(GUI_COORD_Y,0,800,0,799);
		}
	}else if(lcddev.id == 0X1963)//1963为7寸屏 800*480 
	{
		if(lcddev.dir == 0) //竖屏
		{					// 6368 3816
			//GUI_TOUCH_SetOrientation(GUI_SWAP_XY|GUI_MIRROR_Y); 
			GUI_TOUCH_Calibrate(GUI_COORD_X,0,800,0,799); 		
			GUI_TOUCH_Calibrate(GUI_COORD_Y,0,480,0,479);
		}else //横屏
		{
			GUI_TOUCH_Calibrate(GUI_COORD_X,0,800,0,799); 		
			GUI_TOUCH_Calibrate(GUI_COORD_Y,0,480,0,479);
		}
	}else if(lcddev.id == 0X5310 || lcddev.id == 0X6804) //0X5510 0X6804为3.5寸 320x480
	{
		if(lcddev.dir == 0) //竖屏 	
		{							
			GUI_TOUCH_Calibrate(GUI_COORD_X,0,320,3931,226);
			GUI_TOUCH_Calibrate(GUI_COORD_Y,0,480,3812,196);
		}else //横屏
		{
			GUI_TOUCH_SetOrientation(GUI_SWAP_XY|GUI_MIRROR_Y); 
			GUI_TOUCH_Calibrate(GUI_COORD_X,0,320,3931,226);
			GUI_TOUCH_Calibrate(GUI_COORD_Y,0,480,3812,196); 	
		}
	}
	else             //其他屏幕全部默认为2.8寸 320X240
	{
		if(lcddev.dir == 0) //竖屏
		{					
			GUI_TOUCH_Calibrate(GUI_COORD_X,0,lcddev.width,155,3903);
			GUI_TOUCH_Calibrate(GUI_COORD_Y,0,lcddev.height,188,3935);
		}else //横屏
		{
			GUI_TOUCH_SetOrientation(GUI_SWAP_XY|GUI_MIRROR_Y); 
			GUI_TOUCH_Calibrate(GUI_COORD_X,0,240,155,3903); 	
			GUI_TOUCH_Calibrate(GUI_COORD_Y,0,320,188,3935);
		}
	}
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Function description:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;
  (void) LayerIndex;
  (void) pData;
  
  switch (Cmd) {
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    // ...
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/

