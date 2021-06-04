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
File        : GUIConf.c
Purpose     : Display controller initialization
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
#include "malloc.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Define the available number of bytes available for the GUI
//

//#define GUI_NUMBYTES  (50*1024)

#define USE_EXRAM  1//使用外部RAM
//设置EMWIN内存大小
#define GUI_NUMBYTES  (500*1024)
//#define GUI_BLOCKSIZE 0X80  //块大小




/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   available memory for the GUI.
*/


//void GUI_X_Config(void) {
//  //
//  // 32 bit aligned memory area
//  //
//   static U32 aMemory[GUI_NUMBYTES / 4];
//	  

//	//
//  // Assign memory to emWin
//  //
//  GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
//  //
//  // Set default font
//  //
//  GUI_SetDefaultFont(GUI_FONT_6X8);
//}

void GUI_X_Config(void) {
	if(USE_EXRAM) //使用外部RAM
	{
		U32 *aMemory = mymalloc(SRAMEX,GUI_NUMBYTES); //从外部SRAM中分配GUI_NUMBYTES字节的内存
		GUI_ALLOC_AssignMemory((void*)aMemory, GUI_NUMBYTES); //为存储管理系统分配一个存储块
//		GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE); //设置存储快的平均尺寸,该区越大,可用的存储快数量越少
//		GUI_SetDefaultFont(GUI_FONT_6X8); //设置默认字体
	}else  //使用内部RAM
	{
		U32 *aMemory = mymalloc(SRAMIN,GUI_NUMBYTES); //从内部RAM中分配GUI_NUMBYTES字节的内存
		GUI_ALLOC_AssignMemory((U32 *)aMemory, GUI_NUMBYTES); //为存储管理系统分配一个存储块
//		GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE); //设置存储快的平均尺寸,该区越大,可用的存储快数量越少
//		GUI_SetDefaultFont(GUI_FONT_6X8); //设置默认字体
	}
}

/*************************** End of file ****************************/
