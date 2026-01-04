/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： PAL_DataType.h 
开发日期：2024-03-30 
文件功能：定义外设抽象对象需要的数据类型。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __P_A_L__DATA_TYPE_H_
#define __P_A_L__DATA_TYPE_H_

#include "zmCOM/zmCOM_PortIndex.h"

/*************************************************** GPIO *************************************************/
typedef enum 
{
	GPIO_Err_None,
	GPIO_Err_NoPin=-1,
	GPIO_Err_ModeIsNoSurport=-2,
	GPIO_Err_ApplyMemoryFail=-3,
}e_GPIO_ErrorCode_t;

typedef enum 
{
	GPIO_MD_None,
	GPIO_MD_Input,
	GPIO_MD_OD,
	GPIO_MD_PushPull,
	GPIO_MD_TypeMak=0xf,

	GPIO_MD_PullUp=0x10,
	GPIO_MD_PullDown=0x20,
	GPIO_MD_PullMak=0x30,

	GPIO_MD_FuYongFlag=0X80,		//功能复用，此时低4位代表具体类型

}e_GPIO_Mode_t;

typedef enum 
{
	GPIO_Value_LowLevel,
	GPIO_Value_HighLevel,
	GPIO_Value_CHange,
}e_GPIO_Value_t;

typedef enum 
{
	Exit_TMD_None,
	Exit_TMD_RISING,
	Exit_TMD_FALLING,
	Exit_TMD_RISING_FALLING,

	Exit_TMD_Mak=0xf,

	Exit_YXJ_Normal=0,
	Exit_YXJ_High=0x10,

}e_Exit_TriggerMode_t;


typedef unsigned char gpio_t;

/*============================================== 串行通信口 ==============================================================*/
typedef struct 
{
    char comType;
	unsigned char TransFormat;		//传输格式
	union
	{
		unsigned short NetPort;
		struct
		{
			unsigned char ReceiveMode;
			unsigned char SendMode;
		};
		unsigned short I2CNodeAddr;
	};
	union
	{
		unsigned int Baudrate;
		unsigned char IPAddr[4];
	};
} s_com_Setting_t;


/*************************************************** 串口 *********************************************************************/
typedef unsigned char comid_t;

typedef enum 
{
	UART_RWSetting_OK,		
	UART_RWSetting_NotHalUart,		//不是硬件串口
}e_UartRWSettingResult_t;		//读写设置结果

typedef enum 
{
	UART_SENDCBRT_SendOver,
	UART_SENDCBRT_NeedSending,
}e_UartSendCallBackResult_t;

#define Uart_RWSettingIsError(Result)		(Result<UART_RWSetting_OK)		//读写设置结果是错误的

typedef enum
{
	SerialPort_StopBit_1,
	SerialPort_StopBit_2,
	SerialPort_StopBit_1_5,
}e_SerialPortStopBitName_t;

typedef enum
{
	SerialPort_ParityBit_N,
	SerialPort_ParityBit_O,
	SerialPort_ParityBit_E,
}e_SerialPortParityBitName_t;

typedef enum
{
	SerialPort_Error_None,
	SerialPort_Error_Parity=1<<0,
	SerialPort_Error_Framing=1<<1,
	SerialPort_Error_Noise=1<<2,
	SerialPort_Error_Overrun=1<<3,	
}e_SerialPortErrorType_t;

typedef enum
{
	SerialPort_RWMode_Normal,		//常规模式，驱动层分帧，默认为此值
	SerialPort_RWMode_Circle,		//循环收发模式，驱动层根据设定的收发缓存区循环收发
}e_SerialPortRWMode_t;

typedef enum
{
	SerialPort_ByteFormat_Addr_DataBit=0,
	SerialPort_ByteFormat_BitNums_DataBit=3,
	SerialPort_ByteFormat_Addr_StopBit=SerialPort_ByteFormat_Addr_DataBit+SerialPort_ByteFormat_BitNums_DataBit,
	SerialPort_ByteFormat_BitNums_StopBit=2,
	SerialPort_ByteFormat_Addr_ParityBit=SerialPort_ByteFormat_Addr_StopBit+SerialPort_ByteFormat_BitNums_StopBit,
	SerialPort_ByteFormat_BitNums_ParityBit=3,

}e_SerialPortByteFormatBitAddr_t;

typedef union
{
	unsigned char FrameFormat;		//帧格式
	struct
	{
		unsigned char DataBit:SerialPort_ByteFormat_BitNums_DataBit;	//0-4bit,以此类推，5-9bit，其它：自定义
		unsigned char Stop:SerialPort_ByteFormat_BitNums_StopBit;		//0-1位停止位，1-1.5位停止位，2-2位停止位，3-自定义
		unsigned char Parity:SerialPort_ByteFormat_BitNums_ParityBit;		//0-无校验，1-奇校验，2-偶校验，其它-自定义
	};
}u_SerialPortFrameFormat_t;

#define SerialPort_DataBit_Base				4
#define SerialPort_CreateFrameFormatValue(__DataBit,__StopBit,__ParityBit)		((((__DataBit)-SerialPort_DataBit_Base)<<SerialPort_ByteFormat_Addr_DataBit) | \
																				((__StopBit)<<SerialPort_ByteFormat_Addr_StopBit) | \
																				((__ParityBit)<<SerialPort_ByteFormat_BitNums_ParityBit))

/*=========================================== 网络 =================================================================*/
typedef union
{
	unsigned char FrameFormat;		//帧格式
	enum
	{
		Net_FrameFormat_Base,
		Net_FrameFormat_TCP=Net_FrameFormat_Base,
		Net_FrameFormat_UDP,
		Net_FrameFormat_TCPListener,
		Net_FrameFormat_TCPINComing,
		Net_FrameFormat_UDPService,
		Net_FrameFormat_Count,
	};
}u_NetFrameFormat_t;

/*=========================================== I2C =================================================================*/
typedef enum 
{
	I2C_SENDCBRT_SendOver,
	I2C_SENDCBRT_NeedSending,
}e_I2CSendCallBackResult_t;

typedef enum
{
	I2C_ERR_Bus=1<<0,
	I2C_ERR_ARLO=1<<1,
	I2C_ERR_AckFail=1<<2,
	I2C_ERR_OVR=1<<3,
	I2C_ERR_PEC=1<<4,
	I2C_ERR_BaoLiu=1<<5,
	I2C_ERR_TIMEOUT=1<<6,
	I2C_ERR_Msk=0x7f,
	
}e_I2CErrorType_t;

typedef union
{
	unsigned char TransFormat;		//传输格式
	struct
	{
		unsigned char MasterFlag:1;		
		unsigned char Addr16Flag:1;		
//		unsigned char ReadFlag:1;	
		unsigned char OtherFlag:6;		
	};
}u_I2CTransFormat_t;
#define I2C_CreateTransFormatValue(__MasterFlag,__Addr16Flag)		((__MasterFlag) | (__Addr16Flag<<1))


/*************************************************** SPI *********************************************************************/
typedef enum
{
	SPI_RWMode_Normal,		//常规模式，驱动层分帧，默认为此值
	SPI_RWMode_Circle,		//循环收发模式，驱动层根据设定的收发缓存区循环收发
}e_SPIRWMode_t;

typedef union
{
	unsigned char FrameFormat;		//帧格式
	struct
	{
			unsigned char MasterFlag:1;		//主模式标志 1-主，0-从
			unsigned char UseNSS:1;			//1-使用NSS引脚，0-使用普通GPIO引脚
			unsigned char CPOL:1;				//0-CLK引脚常态低电平	1-CLK引脚常态高电平
			unsigned char CPHA:1;				//0-在第一个边沿采样	1-在第二个边沿采样
			unsigned char DataUseBit16:1;		//0-8位数据，1-16位数据
			unsigned char LSBFirst:1;		//低位优先标志，0-先发高位，1-先发低位
			unsigned char REV:2;		//保留
	};
}u_SPIFrameFormat_t;
#define SPI_CreateFrameFormatValue(__MasterFlag,__UseNSS,__CPOL,__CPHA,__DataUseBit16,__LSBFirst)	\
					((__MasterFlag)|((__UseNSS)<<1)|((__CPOL)<<2)|((__CPHA)<<3)|((__DataUseBit16)<<4)|((__LSBFirst)<<5))


#endif


 

