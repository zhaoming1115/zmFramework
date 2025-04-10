/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： HAL_DataType.h 
开发日期：2024-03-24 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __H_A_L__DATA_TYPE_H_
#define __H_A_L__DATA_TYPE_H_

#include <plooc.h>

/*************************************************** 串口 ***********************************************************************/
/*串口自动控制参数*/
typedef struct
{
	unsigned char DEPin;		//485收发控制脚
	unsigned char RxLED;		//接收指示灯，每次接收到一帧数据就变化一次
	unsigned char TxLED;		//发送指示灯，每次发送一帧数据就变化一次
	union
	{
		unsigned char BSPParm;
		struct
		{
			unsigned char EnTxWhenInit:1;		//初始化时使能发送
			unsigned char DELevel:1;	//485发送模式电平
/*收发引脚使用GPIO组
GPIOGroup值 串口Rx_GPIO组ID	串口Tx_GPIO组
	0			0				0
	1			1				0
	2			0				1
	3			1				1
*/
			unsigned char RxGPIOGroup:1;	//收发引脚使用GPIO组,00-
			unsigned char TxGPIOGroup:1;	//收发引脚使用GPIO组,00-
			unsigned char Other:4; //
		};
	};
}s_UartAutoContrlParm_t;

#define Append_UartAutoContrlParm6(DEPinID,DEToSendLevel,UartRxLEDPinID,UartTxLEDPinID,RxPinGroup,TxPinGroup)	\
																						{.DEPin=DEPinID,\
																						.DELevel=DEToSendLevel,\
																						.RxLED=UartRxLEDPinID,\
																						.TxLED=UartTxLEDPinID,\
																						.EnTxWhenInit=0,\
																						.RxGPIOGroup=RxPinGroup,\
																						.TxGPIOGroup=TxPinGroup,\
																						.Other=0}

#define Append_UartAutoContrlParm4(DEPinID,DEToSendLevel,UartRxLEDPinID,UartTxLEDPinID)	Append_UartAutoContrlParm6(DEPinID,DEToSendLevel,UartRxLEDPinID,UartTxLEDPinID,0,0)

#define Append_UartAutoContrlParm2(DEPinID,DEToSendLevel)								Append_UartAutoContrlParm4(DEPinID,DEToSendLevel,ZHL_GPIO_None,ZHL_GPIO_None)

#define Append_UartAutoContrlParm1(DEPinID)												Append_UartAutoContrlParm2(DEPinID,1)

#define Append_UartAutoContrlParm0()													Append_UartAutoContrlParm1(ZHL_GPIO_None)

#define Append_UartAutoContrlParm(...)													FUN_EVAL(Append_UartAutoContrlParm,__VA_ARGS__)(__VA_ARGS__)

#endif


 

