#ifndef _WINDOWDLG_H
#define _WINDOWDLG_H

//#include "WindowDLG.h"
#include "sys.h"
#include "wm.h"
#include "GUI.h"
#include "DIALOG.h"
WM_HWIN CreateWindow(void);

#endif

extern int beat_flag;
extern int height_flag;
void FUN_ICON0Clicked(void);
//void MainTask(void)
//{
//	GUI_Init();

//	CreateFramewin();

//	while (1) {
//		GUI_Delay(100);

//	}
//}
