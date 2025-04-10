/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： HAL_Interface_Sys.h 
开发日期：2025-04-09 
文件功能：zmSys对象的硬件驱动接口。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __H_A_L__INTERFACE__SYS_H_
#define __H_A_L__INTERFACE__SYS_H_

#include <cmsis_compiler.h>
#include "Sys/Sys_DataType.h"
#include "stm32f1xx.h"

#define UH_Sys_UseCustomTick		0
	
#if UH_Sys_UseCustomTick
#define UH_Sys_msToTick(ms)	
#endif

unsigned short UH_Sys_StartKMGou(unsigned short mSecond);
void UH_Sys_EntryLowPower(unsigned int Level);
void UH_Sys_FeedDog(void);
e_Sys_StartFrom_t UH_Sys_GetStartFrom(void);
int UH_Sys_GetUID(__Out unsigned char* UIDData);
#endif


 

