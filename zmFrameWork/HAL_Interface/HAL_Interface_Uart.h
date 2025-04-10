/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： HAL_Interface_Uart.h 
开发日期：2025-03-26 
文件功能：声明异步串口的硬件接口。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/
 #ifndef __HAL_INTERFACE_UART_H_
#define __HAL_INTERFACE_UART_H_
#include "./PAL/PAL_DataType.h"
#include <stdbool.h>

void UH_UART_Init(int UartIndex,const s_com_Setting_t* Setting);
int UH_UART_SendData(int UartIndex,const char* Data,int Length);
int UH_UART_SendChar(int UartIndex,const char data);
void UH_UART_EnableTCInterrupt(int UartIndex);
void UH_UART_DeInit(int UartIndex);
/*
	函数说明：启动接收
	参数说明：
	输入参数：
				UartIndex-串口编号
				*Data-数据接收地址
				Length-接收数据量
	返回值：1-有连续包需要发送，0-本帧已经发送完毕；其它-异常
*/
void UH_UART_StartReceived(int UartIndex,char* Data,int Length);
bool UH_UART_IsOpen(int UartIndex);
bool UH_UART_IsSending(int UartIndex);
unsigned short UH_UART_GetRxOffsetAddr(int UartIndex);		//获取DMA接收缓存指针，若未使用DMA异步接收，需返回-1
unsigned short UH_UART_GetTxOffsetAddr(int UartIndex);	//获取DMA写入缓存指针，若未使用DMA异步接收，需返回-1
int UH_UART_UpdateSetting(int UartNum,const s_com_Setting_t* const NewPortConfig);
int UH_UART_GetSetting(int UartNum,s_com_Setting_t* GetTo);

	//硬件回调


/*****************************************************************************
* 函 数 名： UHC_UART_Connected,UHC_UART_DisConnected
* 函数功能： 连接、断开连接后的回调函数
* 参    数： UartIndex: [输入] -端口号
* 返    回： 无
* 描    述： 主要用于CDC虚拟串口

* 开 发 者：赵明 
* 日    期： 2022-03-31

* 修改历史：
** 1. 新创建

 *****************************************************************************/
extern void UHC_UART_Connected(int UartIndex);
extern void UHC_UART_DisConnected(int UartIndex);

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
extern e_UartSendCallBackResult_t UHC_UART_PacketSended(int UartIndex,char** data,int* datalength);		
/*
	函数说明：帧发送完毕回调函数
	参数说明：
	输入参数：	
		UartIndex-串口编号；
	返回值：无
*/

extern void UHC_UART_DataSended(int UartIndex);

/*
	函数说明：一帧数据数据接收回调函数
	参数说明：
	输入参数：	
		UartIndex-串口编号；
	DataLength-接收到的数据长度
	返回值：无
*/
extern void UHC_UART_DataReceived(int UartIndex,unsigned int DataLength,bool BufferFulledFlag);

extern void UHC_UART_SettingCHanged(int UartIndex,const void* const Setting);
extern void UHC_UART_Error(int UartIndex,e_SerialPortErrorType_t ErrorType);

#endif


 

