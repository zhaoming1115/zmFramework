/*
 * @Author: zhaoming1115 zhaomingaoing@126.com
 * @Date: 2025-11-26 16:31:19
 * @LastEditors: zhaoming1115 zhaomingaoing@126.com
 * @LastEditTime: 2025-11-27 20:41:36
 * @FilePath: \MDK-ARMf:\MDK\zhoujunchao\ZHiBingJi\Public\HAL_Interface\HAL_Interface_SPI.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __HAL_INTERFACE_SPI_H_
#define __HAL_INTERFACE_SPI_H_
#include "./PAL/PAL_DataType.h"
#include <stdbool.h>

int UH_SPI_Init(int PortIndex,const s_com_Setting_t* Setting);
void UH_SPI_DeInit(int PortIndex);
/*
	函数说明：启动接收
	参数说明：
	输入参数：
				SPIIndex-串口编号
				*Data-数据接收地址
				Length-接收数据量
	返回值：1-有连续包需要发送，0-本帧已经发送完毕；其它-异常
*/
int UH_SPI_ReadWrite(int PortIndex,char* WRData,int Length);
bool UH_SPI_IsBusy(int PortIndex);
int UH_SPI_UpdateSetting(int PortIndex,const s_com_Setting_t* const NewPortConfig);
int UH_SPI_GetSetting(int PortIndex,s_com_Setting_t* GetTo);

	//硬件回调

extern void UHC_SPI_SettingCHanged(int PortIndex,const void* const Setting);
extern void UHC_SPI_Error(int PortIndex,e_SerialPortErrorType_t ErrorType);

#endif


 

