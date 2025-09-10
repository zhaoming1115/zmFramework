#include <string.h>
#include "..\Inc\UH_Uart.h"
#include "../Inc/UH_GPIO.h"
#include "..\Inc\UH_DMA.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_rcc.h"

#if ZHL_CDC_Count>0 && (defined(USB) || defined(USB_OTG_FS))
#include "usbd_cdc_if.h"

#endif

#define __Uart_UseTestCounter		0
//#define __Uart_DebugTCUart			3

#pragma pack (4)
typedef struct
{
	unsigned short RCounter;
	unsigned short TCounter;
	char* RBuffer;
	const char* TBuffer;
}JG_RunParmPerUartLX;

typedef struct
{
	JG_RunParmPerUartLX Uart5Parm;
}JG_RunParmLX;

#if __Uart_UseTestCounter
typedef struct
{
	 unsigned int UartRCounter;
    unsigned int UartTCounter1;
    unsigned int UartTCounter2;

}JG_TestCounterLX;
static JG_TestCounterLX g_TestCounter;
#endif


#pragma pack ()

__weak void MX_USART1_UART_Init(unsigned char PinGroupID);
__weak void MX_USART2_UART_Init(unsigned char PinGroupID);
__weak void MX_USART3_UART_Init(unsigned char PinGroupID);

__weak void MX_USART1_UART_Init(unsigned char PinGroupID)
{
}

__weak void MX_USART2_UART_Init(unsigned char PinGroupID)
{
}
__weak void MX_USART3_UART_Init(unsigned char PinGroupID)
{
}

//#define __Uart_UseSendIDLE

#define Uart1_TxDMA					3
#define Uart1_RxDMA					4

#define Uart2_TxDMA					6
#define Uart2_RxDMA					5

#define Uart3_TxDMA					1
#define Uart3_RxDMA					2

#define Uart_GetDMACTCIF(CHannelx)	(DMA_IFCR_CTCIF1<<(4*(CHannelx-1)))
#ifdef UART4
#define Uart4_TxDMA					11
#define Uart4_RxDMA					9
#endif

#if ZHL_Uart_Count<4
#define _InitFunctionTable	MX_USART1_UART_Init,MX_USART2_UART_Init,MX_USART3_UART_Init
#define _g_UartTable			USART1,USART2,USART3		
#define _UartDMATalbe		DMA1,DMA1,DMA1
#define _UARTDMACHannelRTable		Uart1_RxDMA,Uart2_RxDMA,Uart3_RxDMA
#define _UartDMACHannelTTable		Uart1_TxDMA,Uart2_TxDMA,Uart3_TxDMA

#else
#if ZHL_Uart_Count>=4
__weak void MX_UART4_Init(unsigned char PinGroupID);
__weak void MX_UART4_Init(unsigned char PinGroupID)
{

}


#define _UartDMATalbe		DMA1,DMA1,DMA1,DMA2
#define _UARTDMACHannelRTable		Uart1_RxDMA,Uart2_RxDMA,Uart3_RxDMA,Uart4_RxDMA
#define _UartDMACHannelTTable		Uart1_TxDMA,Uart2_TxDMA,Uart3_TxDMA,Uart4_TxDMA
#endif

#if ZHL_Uart_Count==4
#define _InitFunctionTable	MX_USART1_UART_Init,MX_USART2_UART_Init,MX_USART3_UART_Init,MX_UART4_Init
#define _g_UartTable			USART1,USART2,USART3,UART4		

#else
static JG_RunParmLX g_RunParm;

__weak void MX_UART5_Init(unsigned char PinGroupID);
__weak void MX_USART6_UART_Init(unsigned char PinGroupID);
__weak void MX_UART7_Init(unsigned char PinGroupID);
__weak void MX_UART8_Init(unsigned char PinGroupID);

__weak void MX_UART5_Init(unsigned char PinGroupID)
{
}
__weak void MX_USART6_UART_Init(unsigned char PinGroupID)
{
}
__weak void MX_UART7_Init(unsigned char PinGroupID)
{
}
__weak void MX_UART8_Init(unsigned char PinGroupID)
{
}

#define _InitFunctionTable	MX_USART1_UART_Init,MX_USART2_UART_Init,MX_USART3_UART_Init,MX_UART4_Init,MX_UART5_Init,MX_USART6_UART_Init,MX_UART7_Init,MX_UART8_Init
#define _UartTable			USART1,USART2,USART3,UART4,UART5

#define _RunParmNull		{0,0,NULL,NULL}
static JG_RunParmPerUartLX g_UartParmTable[]={_RunParmNull,_RunParmNull,_RunParmNull,_RunParmNull,_RunParmNull,_RunParmNull,_RunParmNull,_RunParmNull};

#endif
#endif

static void UH_UART_PacketSendHandler(int UartIndex);
static void UH_UART_DMASendReport(int CHannelID,int Flag);
static void UH_UART_DMAReceivedReport(int CHannelID,int Flag);

static  USART_TypeDef* const g_UartTable[]={_UartTable};
static  int const g_UARTDMACHannelRTable[]={_UARTDMACHannelRTable};
static  int const g_UartDMACHannelTTable[]={_UartDMACHannelTTable};

#ifdef ZHL_Uart_Use485
#define UH_Uart_UpdateDEPin(UartIndex,DEPin)	
inline static void UH_Uart_GotoReceiveMode(int UartIndex)
{
	const s_UartAutoContrlParm_t* AutoContrlParm=&G_UartAutoContrlParmS[UartIndex];
	UH_GPIO_WritePin(AutoContrlParm->DEPin,AutoContrlParm->DELevel ^ 1);
}

inline static void UH_Uart_GotoSendMode(int UartIndex)
{
	const s_UartAutoContrlParm_t* AutoContrlParm=&G_UartAutoContrlParmS[UartIndex];
	UH_GPIO_WritePin(AutoContrlParm->DEPin,AutoContrlParm->DELevel);
}

	
#else
#define UH_Uart_GotoReceiveMode(UartIndex)
#define UH_Uart_GotoSendMode(UartIndex)

#endif

#ifdef ZHL_Uart_UseRxLED

inline static void UH_Uart_SHowReceivedSignal(int UartIndex)
{
	const s_UartAutoContrlParm_t* AutoContrlParm=&G_UartAutoContrlParmS[UartIndex];
	UH_GPIO_WritePin(AutoContrlParm->RxLED,GPIO_Value_CHange);
}
#else
#define UH_Uart_SHowReceivedSignal(UartIndex)
#endif

#ifdef ZHL_Uart_UseTxLED

inline static void UH_Uart_SHowSendSignal(int UartIndex)
{
	const s_UartAutoContrlParm_t* AutoContrlParm=&G_UartAutoContrlParmS[UartIndex];
	UH_GPIO_WritePin(AutoContrlParm->TxLED,GPIO_Value_CHange);
}
#else
#define UH_Uart_SHowSendSignal(UartIndex)
#endif


inline static void UH_Uart_SignalToSend(int UartIndex)
{
	UH_Uart_GotoSendMode(UartIndex);
	UH_Uart_SHowSendSignal(UartIndex);
}

inline static void UH_Uart_SignalToReceived(int UartIndex)
{
	UH_Uart_GotoReceiveMode(UartIndex);
	UH_Uart_SHowSendSignal(UartIndex);
}

#define UH_Uart_EnableSendOverIRQ(__Uartx)				LL_USART_EnableIT_TC(__Uartx)

static unsigned short g_RxCounts[ZHL_Uart_Count];

void UH_UART_Init(int UartIndex,const s_com_Setting_t* Setting)
{
#define __UartDMAConfig(Dir,LoopMode) {.EN=0,.TCIEN=1,.HalfTCIEN=LoopMode,.ErrIEN=0,.DTDir=Dir,.LoopModeEN=LoopMode,.PICEN=0,.MICEN=1,\
										.PDataWidth=0,.MDataWidth=0,.YouXianJi=0,.M2M=0,.REV=0,.IRQPriority=ZHL_Uart_DMAIRQPriority}
	static const s_DMACHannelInitParm_t _RDMACtrl[]={__UartDMAConfig(0,0),__UartDMAConfig(0,1)};
	static const s_DMACHannelInitParm_t _TDMACtrl[]={__UartDMAConfig(1,0),__UartDMAConfig(1,1)};
	void (*UartInitFunTable[])(unsigned char)={_InitFunctionTable};
	void (*CallBack)(int,int);
	JG_RunParmPerUartLX* _UartParm;

	const s_UartAutoContrlParm_t* AutoContrlParm=&G_UartAutoContrlParmS[UartIndex];
	int id=UartIndex;
	USART_TypeDef * Uart;
	switch(id)
	{
	case 0:
	case 1:
	case 2:
#if ZHL_Uart_Count>=4
	case 3:
#endif
		UartInitFunTable[id](AutoContrlParm->RxGPIOGroup);
		Uart=g_UartTable[id];
		LL_USART_ClearFlag_TC(Uart);
		if(Setting->ReceiveMode==SerialPort_RWMode_Circle )
		{
			CallBack=NULL;
		}
		else
		{
			CallBack=UH_UART_DMAReceivedReport;
		}
		UH_DMA_InitCHannel(g_UARTDMACHannelRTable[id],&_RDMACtrl[Setting->ReceiveMode],(unsigned int)&Uart->DR,CallBack);
		Uart->CR3|=USART_CR3_DMAR;
		if(ZHL_Uart_Index_Debug!=id)
		{
			if(Setting->SendMode)
			{
				CallBack=NULL;
			}
			else
			{
				CallBack=UH_UART_DMASendReport;
			}
			UH_DMA_InitCHannel(g_UartDMACHannelTTable[id],&_TDMACtrl[Setting->SendMode],(unsigned int)&Uart->DR,CallBack);
			Uart->CR3|=USART_CR3_DMAT;
		}
		break;
#if ZHL_Uart_Count>=5
	case 4:
#endif
#if ZHL_Uart_Count>=6
	case 5:
#endif
#if ZHL_Uart_Count>=7
	case 6:
#endif
#if ZHL_Uart_Count>=8
	case 7:
#endif
		memset(&g_UartParmTable[id],0,sizeof(g_UartParmTable[id]));
		Uart=g_UartTable[id];
		UartInitFunTable[id](AutoContrlParm->RxGPIOGroup);
		LL_USART_ClearFlag_TC(Uart);
//		if(Setting->SendMode)
//		{
//			UH_Uart_EnableSendOverIRQ(Uart);
//		}
//		else
		{
			LL_USART_DisableIT_TC(Uart);
		}
		break;

	default:
		return;
		break;
	}

	LL_USART_Disable(Uart);
	UH_UART_UpdateSetting(id, Setting);
	if(Setting->ReceiveMode)
	{
		LL_USART_DisableIT_IDLE(Uart);
	}
	else
	{
		LL_USART_EnableIT_IDLE(Uart);
	}
	if(AutoContrlParm)
	{
		if(ZHL_GPIO_IsValidPin(AutoContrlParm->DEPin))
		{
			UH_GPIO_SetMode(AutoContrlParm->DEPin,GPIO_MD_PushPull);
			if(AutoContrlParm->EnTxWhenInit)
			{
				UH_Uart_GotoSendMode(id);
			}
		}
		UH_GPIO_SetMode(AutoContrlParm->TxLED,GPIO_MD_PushPull);
		UH_GPIO_SetMode(AutoContrlParm->RxLED,GPIO_MD_PushPull);
	}
	LL_USART_Enable(Uart);
	
#if __Uart_UseTestCounter
	memset(&g_TestCounter,0,sizeof(g_TestCounter));
#endif
}

int UH_UART_SendData(int UartIndex,const char* Data,int Length)
{
	int DMAChannel;
	JG_RunParmPerUartLX* _UartParm;
	USART_TypeDef * Uart;
#if __Uart_UseTestCounter
	g_TestCounter.UartTCounter1+=Length;
#endif

	switch(UartIndex)
	{
	case 0:
	case 1:
	case 2:
#if ZHL_Uart_Count>=4
	case 3:
#endif
		Uart=g_UartTable[UartIndex];
		UH_Uart_SignalToSend(UartIndex);

#ifdef __Uart_UseSendIDLE
		LL_USART_EnableDirectionTx(Uart);
#endif
		DMAChannel=g_UartDMACHannelTTable[UartIndex];
		UH_DMA_SuspendTrans(DMAChannel);
		LL_USART_ClearFlag_TC(Uart);
		UH_DMA_ContinueTrans(DMAChannel,(unsigned int)Data,Length);
		UH_Uart_EnableSendOverIRQ(Uart);
//		LL_USART_EnableDMAReq_TX(Uart);
		return Length;
		break;

#if ZHL_Uart_Count>=5
	case 4:
#endif
#if ZHL_Uart_Count>=6
	case 5:
#endif
#if ZHL_Uart_Count>=7
	case 6:
#endif
#if ZHL_Uart_Count>=8
	case 7:
#endif
		Uart=g_UartTable[UartIndex];
		UH_Uart_SignalToSend(UartIndex);
		_UartParm=&g_UartParmTable[UartIndex];

		_UartParm->TBuffer=Data;
		_UartParm->TCounter=Length;
		LL_USART_DisableDirectionTx(Uart);
		LL_USART_EnableDirectionTx(Uart);
		LL_USART_EnableIT_TXE(Uart);
		UH_Uart_EnableSendOverIRQ(Uart);
		return Length;
	default:
#if ZHL_CDC_Count
		UartIndex-=ZHL_Uart_Count;
		if(UartIndex<ZHL_CDC_Count)
		{
			CDC_Transmit_FS(UartIndex,Data,Length);
		}
#endif
		break;
	}
	return 0;
}

int UH_UART_SendChar(int UartIndex,const char data)
{
	USART_TypeDef * Uart;
	if(UartIndex<ZHL_Uart_Count)
	{
		UH_Uart_SignalToSend(UartIndex);
		Uart=g_UartTable[UartIndex];
		Uart->DR=data;
	}
#if ZHL_CDC_Count
	else
	{
		UartIndex-=ZHL_Uart_Count;
		if(UartIndex<ZHL_CDC_Count)
		{
			CDC_Transmit_FS(UartIndex,&data,1);
		}
	}
#endif
	return 0;
}

void UH_UART_DeInit(int UartIndex)
{
	if(UartIndex>=ZHL_Uart_Count) return;
	if(UartIndex<4)
	{
		UH_DMA_SuspendTrans(g_UartDMACHannelTTable[UartIndex]);
		UH_DMA_SuspendTrans(g_UARTDMACHannelRTable[UartIndex]);
	}
	USART_TypeDef * Uart;

	Uart=g_UartTable[UartIndex];
	LL_USART_ClearFlag_IDLE(Uart);
	LL_USART_ClearFlag_TC(Uart);
	LL_USART_DisableIT_TC(Uart);
	LL_USART_EnableIT_IDLE(Uart);
	LL_USART_DisableDMAReq_RX(Uart);
	LL_USART_DisableDMAReq_TX(Uart);
	LL_USART_Disable(Uart);
}

void UH_UART_StartReceived(int UartIndex,char* Data,int Length)
{
	JG_RunParmPerUartLX* _UartParm;
	int DMAChannel;
	USART_TypeDef * Uart;

	switch (UartIndex)
	{
	case 0:
	case 1:
	case 2:
#if ZHL_Uart_Count>=4
	case 3:
#endif
		UH_Uart_GotoReceiveMode(UartIndex);
		DMAChannel=g_UARTDMACHannelRTable[UartIndex];
		g_RxCounts[UartIndex]=Length;
		UH_DMA_ContinueTrans(DMAChannel,(unsigned int)Data,Length);
		break;

#if ZHL_Uart_Count>=5
	case 4:
#endif
#if ZHL_Uart_Count>=6
	case 5:
#endif
#if ZHL_Uart_Count>=7
	case 6:
#endif
#if ZHL_Uart_Count>=8
	case 7:
#endif
		UH_Uart_GotoReceiveMode(UartIndex);
		Uart=g_UartTable[UartIndex];
		g_RxCounts[UartIndex]=Length;
		_UartParm=&g_UartParmTable[UartIndex];
		_UartParm->RCounter=Length;
		_UartParm->RBuffer=Data;
		LL_USART_EnableIT_RXNE(Uart);
		break;

	default:
#if ZHL_CDC_Count
		UartIndex-=ZHL_Uart_Count;
		if(UartIndex<ZHL_CDC_Count)
		{
			CDC_Receive_Start(UartIndex,Data,Length);
		}
#endif
		return;
		break;
	}

}

bool UH_UART_IsOpen(int UartIndex)
{
	if(UartIndex<ZHL_Uart_Count)
	{
		return LL_USART_IsEnabled(g_UartTable[UartIndex]);;
	}
#if ZHL_CDC_Count
	else
	{
		return CDC_USBIsConnected(UartIndex-ZHL_Uart_Count,NULL);
	}
#else
	return false;
#endif
}

bool UH_UART_IsSending(int UartIndex)
{
	if(UartIndex<ZHL_Uart_Count)
	{
		return LL_USART_IsEnabledIT_TC(g_UartTable[UartIndex]);
	}
#if ZHL_CDC_Count
	else
	{
		return CDC_Transmit_IsSending(UartIndex);
	}
#else
	return false;
#endif
}

inline static int UH_Uart_GetUartFlag(USART_TypeDef* Uart)
{
	return Uart->SR & 0xf;
}

inline static e_SerialPortErrorType_t UH_Uart_ConvErrorData(int ErrorFlag)
{
	return ErrorFlag;
}

inline static void UH_UART_DataErrored(int UartIndex,int ErrorFlag)
{
	UH_DMA_SuspendTrans(g_UARTDMACHannelRTable[UartIndex]);
	UHC_UART_Error(UartIndex,UH_Uart_ConvErrorData(ErrorFlag));
}


inline static void UH_UART_DataReceived(int UartIndex,int DataLength)
{
	UH_DMA_SuspendTrans(g_UARTDMACHannelRTable[UartIndex]);
	UH_Uart_SHowReceivedSignal(UartIndex);
	
	UHC_UART_DataReceived(UartIndex,g_RxCounts[UartIndex]-DataLength,false);
#if __Uart_UseTestCounter
	g_TestCounter.UartRCounter+=g_RxCounts[UartIndex]-DataLength;
#endif
}

inline static void UH_Uart_ClearErrorData(USART_TypeDef* Uart)
{
  __IO uint32_t tmpreg;
  tmpreg = Uart->SR;
  (void) tmpreg;
  tmpreg = Uart->DR;
  (void) tmpreg;	
}

void Uart_IRQHandler(size_t UartIndex)
{
	USART_TypeDef* Uart;
	Uart=g_UartTable[UartIndex];

	if (LL_USART_IsEnabledIT_TC(Uart) && LL_USART_IsActiveFlag_TC(Uart))
	{
		LL_USART_ClearFlag_TC(Uart);
		LL_USART_DisableIT_TC(Uart);		
//		UH_DMA_SuspendTrans(g_UartDMACHannelTTable[UartIndex]);
#ifdef __Uart_UseSendIDLE
		LL_USART_DisableDirectionTx(Uart);
#endif
		UH_Uart_SignalToReceived(UartIndex);
		UHC_UART_DataSended(UartIndex);
	}

	if (LL_USART_IsEnabledIT_IDLE(Uart) && LL_USART_IsActiveFlag_IDLE(Uart))
	{
		int ErrorFlag=UH_Uart_GetUartFlag(Uart);

		if(ErrorFlag)
		{
			UH_Uart_ClearErrorData(Uart);
			UH_UART_DataErrored(UartIndex,ErrorFlag);
		}
		else
		{
			LL_USART_ClearFlag_IDLE(Uart);
			int _RemLen=UH_DMA_GetRemLength(g_UARTDMACHannelRTable[UartIndex]);
			UH_UART_DataReceived(UartIndex,UH_DMA_GetRemLength(g_UARTDMACHannelRTable[UartIndex]));
		}
	}
}

void USART1_IRQHandler(void)
{
	Uart_IRQHandler(0);
}

void USART2_IRQHandler(void)
{
	Uart_IRQHandler(1);
}

void USART3_IRQHandler(void)
{
	Uart_IRQHandler(2);
}

#if ZHL_Uart_Count>=4
void UART4_IRQHandler(void)
{
	Uart_IRQHandler(3);
}


#endif

static void UH_UART_DMASendReport(int CHannelID,int Flag)
{
	int i;
	for (i=0;i<sizeof(g_UartDMACHannelTTable);i++)
	{
		if (g_UartDMACHannelTTable[i]==CHannelID)
		{
			break;
		}
	}

	if ((Flag&DMA_FLAG_TC1) && i<sizeof(g_UartDMACHannelTTable))	//全传输中断
	{
		UH_DMA_SuspendTrans(CHannelID);
		UH_UART_PacketSendHandler(i);
	}
}

static void UH_UART_DMAReceivedReport(int CHannelID,int Flag)
{
	int i;
	for (i=0;i<sizeof(g_UARTDMACHannelRTable);i++)
	{
		if (g_UARTDMACHannelRTable[i]==CHannelID)
		{
			break;
		}
	}
	if ((Flag&DMA_FLAG_TC1) && i<sizeof(g_UARTDMACHannelRTable))	//全传输中断
	{
		UH_DMA_SuspendTrans(CHannelID);
		UHC_UART_DataReceived(i,g_RxCounts[i],true);
#if __Uart_UseTestCounter
		g_TestCounter.UartRCounter+=g_RxCounts[i];
#endif
	}
}

#if ZHL_Uart_Count>=5
void Uart_IRQHandler_Add(size_t UartIndex)
{
	USART_TypeDef* Uart=g_UartTable[UartIndex];
	if (LL_USART_IsActiveFlag_TXE(Uart) && LL_USART_IsEnabledIT_TXE(Uart))
	{
		char data;
		if(g_UartParmTable[UartIndex].TCounter==0)
		{
			if(LL_USART_IsEnabledIT_TC(Uart))		//循环模式下TC中断不会使能
			{
				LL_USART_DisableIT_TXE(Uart);
				UH_UART_PacketSendHandler(UartIndex);
			}
			else		//Uart5暂时不支持循环发送，先用非循环模式的程序处理
			{
				LL_USART_DisableIT_TXE(Uart);
				UH_UART_PacketSendHandler(UartIndex);
			}
		}
		else
		{
			Uart->DR=*g_UartParmTable[UartIndex].TBuffer++;
			g_UartParmTable[UartIndex].TCounter--;
		}
	}
	if (LL_USART_IsActiveFlag_TC(Uart) && LL_USART_IsEnabledIT_TC(Uart))
	{
		LL_USART_ClearFlag_TC(Uart);
		LL_USART_DisableIT_TC(Uart);
#ifdef __Uart_UseSendIDLE
		LL_USART_DisableDirectionTx(Uart);
#endif
		UH_Uart_SignalToReceived(UartIndex);
		UHC_UART_DataSended(UartIndex);
	}
	if (LL_USART_IsEnabledIT_RXNE(Uart)&&LL_USART_IsActiveFlag_RXNE(Uart))
	{
		int ErrorFlag=UH_Uart_GetUartFlag(Uart);

		if(ErrorFlag)
		{
			LL_USART_ClearFlag_RXNE(Uart);
			UH_Uart_ClearErrorData(Uart);
			UHC_UART_Error(UartIndex,UH_Uart_ConvErrorData(ErrorFlag));
		}
		else
		{
			do
			{
				*g_UartParmTable[UartIndex].RBuffer++=Uart->DR;
				g_UartParmTable[UartIndex].RCounter--;
				if(g_UartParmTable[UartIndex].RCounter<=0)
				{
					if(LL_USART_IsEnabledIT_IDLE(Uart))		//循环模式下IDLE不会使能
					{
						UHC_UART_DataReceived(UartIndex,g_RxCounts[UartIndex],true);
					}
					else
					{
						g_UartParmTable[UartIndex].RCounter=g_RxCounts[UartIndex];
						g_UartParmTable[UartIndex].RBuffer-=g_UartParmTable[UartIndex].RCounter;
					}
				}
			}while(LL_USART_IsActiveFlag_RXNE(Uart));
		}
	}
	if (LL_USART_IsEnabledIT_IDLE(Uart)&&LL_USART_IsActiveFlag_IDLE(Uart))
	{
		LL_USART_DisableIT_RXNE(Uart);
//		UHC_UART_DataReceived(UartIndex,g_RxCounts[UartIndex]-g_UartParmTable[UartIndex].RCounter);
		int ErrorFlag=UH_Uart_GetUartFlag(Uart);

		if(ErrorFlag)
		{
			UH_Uart_ClearErrorData(Uart);
			UH_UART_DataErrored(UartIndex,ErrorFlag);
		}
		else
		{
			LL_USART_ClearFlag_IDLE(Uart);
			UH_Uart_SHowReceivedSignal(UartIndex);
			UHC_UART_DataReceived(UartIndex,g_RxCounts[UartIndex]-g_UartParmTable[UartIndex].RCounter,false);
		}
	}
}

void UART5_IRQHandler(void)
{
	Uart_IRQHandler_Add(4);
}
void USART6_IRQHandler(void)
{
	Uart_IRQHandler_Add(5);
}
void UART7_IRQHandler(void)
{
	Uart_IRQHandler_Add(6);
}
void UART8_IRQHandler(void)
{
	Uart_IRQHandler_Add(7);
}
#endif


static void UH_UART_PacketSendHandler(int UartIndex)
{
	char* datap;
	int length;
	USART_TypeDef* Uart=g_UartTable[UartIndex];
	
	
	e_UartSendCallBackResult_t RT=UHC_UART_PacketSended(UartIndex,&datap,&length);

	if (RT==UART_SENDCBRT_NeedSending)
	{
		UH_UART_SendData(UartIndex,datap,length);
	}
}

void UH_UART_SetBoTeLv(int UartNum,int BoTeLv)
{
	LL_RCC_ClocksTypeDef rcc_clocks;
	USART_TypeDef* Uart;
	int tmp;
	LL_RCC_GetSystemClocksFreq(&rcc_clocks);

	switch(UartNum)
	{
	case 0:
		tmp=rcc_clocks.PCLK2_Frequency;
		break;

	case 1:
	case 2:
#if ZHL_Uart_Count>=4
	case 3:
#endif
#if ZHL_Uart_Count>=5
	case 4:
#endif
#if ZHL_Uart_Count>=6
	case 5:
#endif
#if ZHL_Uart_Count>=7
	case 6:
#endif
#if ZHL_Uart_Count>=8
	case 7:
#endif
		tmp=rcc_clocks.PCLK1_Frequency;
		break;

	default:
		return;
		break;

	}
	Uart=g_UartTable[UartNum];
	LL_USART_SetBaudRate(Uart,tmp,BoTeLv);

}

int UH_UART_UpdateSetting(int UartNum,const s_com_Setting_t* const NewPortConfig)
{
	LL_RCC_ClocksTypeDef rcc_clocks;
	USART_TypeDef* Uart;
	int tmp;
	LL_RCC_GetSystemClocksFreq(&rcc_clocks);

	switch(UartNum)
	{
	case 0:
		tmp=rcc_clocks.PCLK2_Frequency;
		break;

	case 1:
	case 2:
#if ZHL_Uart_Count>=4
	case 3:
#endif
#if ZHL_Uart_Count>=5
	case 4:
#endif
#if ZHL_Uart_Count>=6
	case 5:
#endif
#if ZHL_Uart_Count>=7
	case 6:
#endif
#if ZHL_Uart_Count>=8
	case 7:
#endif
		tmp=rcc_clocks.PCLK1_Frequency;
		break;

	default:
		return UART_RWSetting_NotHalUart;
		break;

	}
	Uart=g_UartTable[UartNum];
	unsigned int EnableFlag=LL_USART_IsEnabled(Uart);
	LL_USART_Disable(Uart);
	LL_USART_SetBaudRate(Uart,tmp,NewPortConfig->Baudrate);
	const u_SerialPortFrameFormat_t* FrameForme=(const u_SerialPortFrameFormat_t*)&NewPortConfig->TransFormat;
	unsigned char DataBits=FrameForme->DataBit;
	
	switch(FrameForme->Parity)
	{
		case 0:
			LL_USART_SetParity(Uart,LL_USART_PARITY_NONE);
			break;

		case 1:
			LL_USART_SetParity(Uart,LL_USART_PARITY_ODD);
			DataBits++;
			break;

		case 2:
			LL_USART_SetParity(Uart,LL_USART_PARITY_EVEN);
			DataBits++;
			break;
		
		default:
			break;
	}

	switch(DataBits)
	{
		case 4:
			LL_USART_SetDataWidth(Uart,LL_USART_DATAWIDTH_8B);
			break;
	
		case 5:
			LL_USART_SetDataWidth(Uart,LL_USART_DATAWIDTH_9B);
			break;
					
		default:
			break;
	}

	switch(FrameForme->Stop)
	{
		case SerialPort_StopBit_1:
			LL_USART_SetStopBitsLength(Uart,LL_USART_STOPBITS_1);
			break;

		case SerialPort_StopBit_1_5:
			LL_USART_SetStopBitsLength(Uart,LL_USART_STOPBITS_1_5);
			break;

		case SerialPort_StopBit_2:
			LL_USART_SetStopBitsLength(Uart,LL_USART_STOPBITS_2);
			break;
		
		default:
			break;
	}	
	if(EnableFlag) LL_USART_Enable(Uart);
	return UART_RWSetting_OK;
}

int UH_UART_GetSetting(int UartNum,s_com_Setting_t* GetTo)
{
	LL_RCC_ClocksTypeDef rcc_clocks;
	USART_TypeDef* Uart;
	int tmp;
	LL_RCC_GetSystemClocksFreq(&rcc_clocks);

	switch(UartNum)
	{
	case 0:
		tmp=rcc_clocks.PCLK2_Frequency;
		break;

	case 1:
	case 2:
#if ZHL_Uart_Count>=4
	case 3:
#endif
#if ZHL_Uart_Count>=5
	case 4:
#endif
#if ZHL_Uart_Count>=6
	case 5:
#endif
#if ZHL_Uart_Count>=7
	case 6:
#endif
#if ZHL_Uart_Count>=8
	case 7:
#endif
		tmp=rcc_clocks.PCLK1_Frequency;
		break;

	default:
		return UART_RWSetting_NotHalUart;
		break;

	}
	Uart=g_UartTable[UartNum];
	GetTo->Baudrate= LL_USART_GetBaudRate(Uart,tmp);
	u_SerialPortFrameFormat_t* FrameForme=(u_SerialPortFrameFormat_t*)&GetTo->TransFormat;
	FrameForme->DataBit=(LL_USART_GetDataWidth(Uart)==LL_USART_DATAWIDTH_8B)?4:5;

	switch(LL_USART_GetParity(Uart))
	{
		case LL_USART_PARITY_NONE:
			FrameForme->Parity=0;
			break;

		case LL_USART_PARITY_ODD:
			FrameForme->Parity=1;
			FrameForme->DataBit--;
			break;

		case LL_USART_PARITY_EVEN:
			FrameForme->Parity=2;
			FrameForme->DataBit--;
			break;
		
		default:
			break;
	}


	switch(LL_USART_GetStopBitsLength(Uart))
	{
		case LL_USART_STOPBITS_1:
			FrameForme->Stop=0;
			break;

		case LL_USART_STOPBITS_1_5:
			FrameForme->Stop=1;
			break;

		case LL_USART_STOPBITS_2:
			FrameForme->Stop=2;
			break;
		
		default:
			break;
	}	
	return UART_RWSetting_OK;
}


unsigned short UH_UART_GetRxOffsetAddr(int UartIndex)
{
	switch(UartIndex)
	{
	case 0:
	case 1:
	case 2:
#if ZHL_Uart_Count>=4
	case 3:
		return g_RxCounts[UartIndex]-UH_DMA_GetRemLength(g_UARTDMACHannelRTable[UartIndex]);
		break;
#endif
#if ZHL_Uart_Count>=5
	case 4:
#endif
#if ZHL_Uart_Count>=6
	case 5:
#endif
#if ZHL_Uart_Count>=7
	case 6:
#endif
#if ZHL_Uart_Count>=8
	case 7:
#endif
		return g_RxCounts[UartIndex]-g_UartParmTable[UartIndex].RCounter;
		break;
	
	default:
#if ZHL_CDC_Count
		UartIndex-=ZHL_Uart_Count;
		if(UartIndex<ZHL_CDC_Count)
		{
			return CDC_GetRecivingCount(UartIndex);
		}
#endif
		return 0;
		break;

	}
	
}


/*****************************************************************************
* 函 数 名： UH_UART_GetTxOffsetAddr
* 函数功能： 获取发送缓存区偏移指针，仅用于循环发送模式
* 参    数： UartIndex: [输入] ，串口号
* 返    回： 偏移字节量
* 描    述： 本函数预留，待完成

* 开 发 者：赵明 
* 日    期： 2022-03-23

* 修改历史：
** 1. 新创建

 *****************************************************************************/
unsigned short UH_UART_GetTxOffsetAddr(int UartIndex)	
{
	switch(UartIndex)
	{
	case 0:
	case 1:
	case 2:
#if ZHL_Uart_Count>=4
	case 3:
		return UH_DMA_GetRemLength(g_UartDMACHannelTTable[UartIndex]);
		break;
#endif
#if ZHL_Uart_Count>=5
	case 4:
#endif
#if ZHL_Uart_Count>=6
	case 5:
#endif
#if ZHL_Uart_Count>=7
	case 6:
#endif
#if ZHL_Uart_Count>=8
	case 7:
#endif
		return g_RunParm.Uart5Parm.TCounter;
		break;
	
	default:
#if ZHL_CDC_Count
		UartIndex-=ZHL_Uart_Count;
		if(UartIndex<ZHL_CDC_Count)
		{
			return CDC_GetSendingCount(UartIndex);
		}
#endif
		return 0;
		break;

	}
}



