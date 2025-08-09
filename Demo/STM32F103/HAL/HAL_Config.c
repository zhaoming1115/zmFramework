/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： HAL_Config.c 
开发日期：2025-04-09 
文件功能：

描    述：

依赖接口：

提供接口：
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/
#include "HAL_Config.h"
#include "HAL_DataType.h"
#include "C_Extended.h"
#include "Inc/UH_Flash.h"

const s_UartAutoContrlParm_t G_UartAutoContrlParmS[ZHL_Uart_Count]={Append_UartAutoContrlParm(ZHL_GPIO_None,1,ZHL_GPIO_None,ZHL_GPIO_None),
																	Append_UartAutoContrlParm(ZHL_GPIO_None,1,ZHL_GPIO_None,ZHL_GPIO_None),
																	Append_UartAutoContrlParm(ZHL_GPIO_None,1,ZHL_GPIO_None,ZHL_GPIO_None),
																	Append_UartAutoContrlParm(ZHL_GPIO_None,1,ZHL_GPIO_None,ZHL_GPIO_None),
																	Append_UartAutoContrlParm(ZHL_GPIO_None,1,ZHL_GPIO_None,ZHL_GPIO_None)};

const unsigned char G_I2CGPIOGroupS[ZHL_I2C_Count]={1,0};
													
void Storage_AddDevices(int (*AddStorageBlock)(const c_StorageBlock_t*  StorageBlock))
{
	AddStorageBlock(&g_InnerFlash);
}
