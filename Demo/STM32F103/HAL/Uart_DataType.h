/*****************************************************************************
----------------------------------开发者信息---------------------------------
版权所有：赵明
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： Uart_DataType.h 
开发日期：2024-03-24 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __UART__DATA_TYPE_H_
#define __UART__DATA_TYPE_H_

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

typedef union
{
	unsigned char FrameGormat;		//帧格式
	struct
	{
		unsigned char DataBit:3;	//0-4bit,以此类推，5-9bit，其它：自定义
		unsigned char Stop:2;		//0-1位停止位，1-1.5位停止位，2-2位停止位，3-自定义
		unsigned char Parity:3;		//0-无校验，1-奇校验，2-偶校验，其它-自定义
	};
}u_UartFrameFormat_t;


#endif


 

