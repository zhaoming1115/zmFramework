/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： HAL_Interface_PWM.h 
开发日期：2025-02-09 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __H_A_L__INTERFACE__P_W_M_H_
#define __H_A_L__INTERFACE__P_W_M_H_

#include "./PAL/PAL_DataType.h"
#include <stdbool.h>

void UH_PWM_Init(unsigned int channelindex,unsigned int pinlv,unsigned int jingdu);
void UH_PWM_Start(unsigned int channelindex);
void UH_PWM_Stop(unsigned int channelindex);
void UH_PWM_SetMaiKuan(unsigned int channelindex,unsigned int maikuan);
unsigned int UH_PWM_GetMaiKuan(unsigned int channelindex);
unsigned int UH_PWM_GetJingDu(unsigned int channelindex);

#endif


 

