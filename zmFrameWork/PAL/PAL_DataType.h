/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� PAL_DataType.h 
�������ڣ�2024-03-30 
�ļ����ܣ�����������������Ҫ���������͡�

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

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

	GPIO_MD_FuYongFlag=0X80,		//���ܸ��ã���ʱ��4λ�����������

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

/*============================================== ����ͨ�ſ� ==============================================================*/
typedef struct 
{
    char comType;
	unsigned char TransFormat;		//�����ʽ
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


/*************************************************** ���� *********************************************************************/
typedef unsigned char comid_t;

typedef enum 
{
	UART_RWSetting_OK,		
	UART_RWSetting_NotHalUart,		//����Ӳ������
}e_UartRWSettingResult_t;		//��д���ý��

typedef enum 
{
	UART_SENDCBRT_SendOver,
	UART_SENDCBRT_NeedSending,
}e_UartSendCallBackResult_t;

#define Uart_RWSettingIsError(Result)		(Result<UART_RWSetting_OK)		//��д���ý���Ǵ����

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
	SerialPort_RWMode_Normal,		//����ģʽ���������֡��Ĭ��Ϊ��ֵ
	SerialPort_RWMode_Circle,		//ѭ���շ�ģʽ������������趨���շ�������ѭ���շ�
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
	unsigned char FrameFormat;		//֡��ʽ
	struct
	{
		unsigned char DataBit:SerialPort_ByteFormat_BitNums_DataBit;	//0-4bit,�Դ����ƣ�5-9bit���������Զ���
		unsigned char Stop:SerialPort_ByteFormat_BitNums_StopBit;		//0-1λֹͣλ��1-1.5λֹͣλ��2-2λֹͣλ��3-�Զ���
		unsigned char Parity:SerialPort_ByteFormat_BitNums_ParityBit;		//0-��У�飬1-��У�飬2-żУ�飬����-�Զ���
	};
}u_SerialPortFrameFormat_t;

#define SerialPort_DataBit_Base				4
#define SerialPort_CreateFrameFormatValue(__DataBit,__StopBit,__ParityBit)		((((__DataBit)-SerialPort_DataBit_Base)<<SerialPort_ByteFormat_Addr_DataBit) | \
																				((__StopBit)<<SerialPort_ByteFormat_Addr_StopBit) | \
																				((__ParityBit)<<SerialPort_ByteFormat_BitNums_ParityBit))

/*=========================================== ���� =================================================================*/
typedef union
{
	unsigned char FrameFormat;		//֡��ʽ
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
	unsigned char TransFormat;		//�����ʽ
	struct
	{
		unsigned char MasterFlag:1;		
		unsigned char Addr16Flag:1;		
//		unsigned char ReadFlag:1;	
		unsigned char OtherFlag:6;		
	};
}u_I2CTransFormat_t;
#define I2C_CreateTransFormatValue(__MasterFlag,__Addr16Flag)		((__MasterFlag) | (__Addr16Flag<<1))

#endif


 

