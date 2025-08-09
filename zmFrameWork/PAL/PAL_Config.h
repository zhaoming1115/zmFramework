/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： PAL_Config.h 
开发日期：2024-03-25 
文件功能：定义外设抽象对象需要的配置数据。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __P_A_L__CONFIG_H_
#define __P_A_L__CONFIG_H_
#include <stdio.h>
#include "../App_cfg.h"

/*========================================================== GPIO =================================================================*/
#define GPIO_PrintError								printf
#define GPIO_None									0xff

/*========================================================== 数字通信口 =================================================================*/
#define COM_PrintfError								printf
#define COM_FrameSizeLevel_Default					8
#define COM_QueueFrameCount_Default					4

#define SerialPort_PrintfError						COM_PrintfError
#define SerialPort_FrameSizeLevel_Default			8
#define SerialPort_QueueFrameCount_Default			4

#define I2C_PrintfError								COM_PrintfError
#define I2C_FrameSizeLevel_Default					5
#define I2C_QueueFrameCount_Default					1

/*========================================================== 存储 =================================================================*/
#define Storage_PrintfError							printf

#endif


 

