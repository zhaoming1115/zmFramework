/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： HAL_Interface_GPIO.h 
开发日期：2025-04-09 
文件功能：声明zmGPIOManager对象的硬件驱动接口。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __H_A_L__INTERFACE__G_P_I_O_H_
#define __H_A_L__INTERFACE__G_P_I_O_H_
#include "PAL/PAL_DataType.h"


int UH_GPIO_SetMode(gpio_t GPIOx,e_GPIO_Mode_t mode);
int UH_GPIO_WritePin(gpio_t GPIOx,e_GPIO_Value_t Value);
e_GPIO_Value_t UH_GPIO_ReadPin(gpio_t GPIOx);
void UH_GPIO_Init(void);
int UH_GPIO_SetExitIT(gpio_t GPIOx,e_Exit_TriggerMode_t TriggerMode);
int UH_GPIO_ClearExitIT(gpio_t GPIOx);


extern void UHC_GPIO_InHandler(gpio_t GPIOx);

#endif


 

