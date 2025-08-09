/*****************************************************************************
----------------------------------开发者信息---------------------------------
版权所有：赵明
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： UH_Uart.h 
开发日期：2024-03-24 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __HAL_INTERFACE_I2C_H_
#define __HAL_INTERFACE_I2C_H_
#include "./PAL/PAL_DataType.h"
#include <stdbool.h>

void UH_I2C_Init(int PortIndex,const s_com_Setting_t* Setting);
int UH_I2C_TransData(int PortIndex,const s_com_Setting_t* const I2CSetting,const char* Data,int Length);
void UH_I2C_DeInit(int PortIndex);
bool UH_I2C_IsOpen(int PortIndex);
bool UH_I2C_IsBusy(int PortIndex);
int UH_I2C_UpdateSetting(int PortIndex,const s_com_Setting_t* const NewPortConfig);
int UH_I2C_GetSetting(int PortIndex,s_com_Setting_t* GetTo);

	//硬件回调


/*****************************************************************************
* 函 数 名： UHC_I2C_Connected,UHC_I2C_DisConnected
* 函数功能： 连接、断开连接后的回调函数
* 参    数： UartIndex: [输入] -端口号
* 返    回： 无
* 描    述： 主要用于CDC虚拟串口

* 开 发 者：赵明 
* 日    期： 2022-03-31

* 修改历史：
** 1. 新创建

 *****************************************************************************/
extern void UHC_I2C_Connected(int PortIndex);
extern void UHC_I2C_DisConnected(int PortIndex);

/*
	函数说明：帧内分包发送完毕回调函数
	参数说明：
	输入参数：	
		UartIndex-串口编号；
	输出参数：
	**data-用于存储下一包数据的指针
	*datalength-用于存储下一包数据的数量
	返回值：1-有连续包需要发送，0-本帧已经发送完毕；其它-异常
*/
extern e_I2CSendCallBackResult_t UHC_I2C_PacketSended(int PortIndex,char** data,int* datalength);		
/*
	函数说明：帧发送完毕回调函数
	参数说明：
	输入参数：	
		UartIndex-串口编号；
	返回值：无
*/

extern void UHC_I2C_DataSended(int PortIndex);

/*
	函数说明：一帧数据数据接收回调函数
	参数说明：
	输入参数：	
		UartIndex-串口编号；
	DataLength-接收到的数据长度
	返回值：无
*/
extern void UHC_I2C_DataReceived(int PortIndex,unsigned int DataLength);

extern void UHC_I2C_SettingCHanged(int PortIndex,const void* const Setting);
extern void UHC_I2C_Error(int PortIndex,e_I2CErrorType_t ErrorType);

#endif


 

