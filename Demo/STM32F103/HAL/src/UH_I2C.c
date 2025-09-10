#include <string.h>
#include "..\Inc\UH_I2C.h"
#include <stdio.h>
#include "stm32f1xx_ll_i2c.h"
#include "stm32f1xx_ll_RCC.h"

typedef struct
{
	unsigned short RWCount;
	unsigned short RWCounter;
	union
	{
		char* RBuffer;
		const char* TBuffer;
	};
}s_RunParmPerI2C_t;

__weak void MX_I2C1_Init(unsigned char PinGroupID);
__weak void MX_I2C2_Init(unsigned char PinGroupID);

//#define I2CPrintf			printf
#define I2CPrintf(...)			

#define _InitFunctionTable	MX_I2C1_Init,MX_I2C2_Init
#define _I2CTable			I2C1,I2C2
#define UH_I2C_IsReceiveOneByte(__Parm)			(__Parm->RWCount==2)
#define UH_I2C_IsReceiveTwoByte(__Parm)			(__Parm->RWCount==3)

static  I2C_TypeDef* const g_I2CTable[]={_I2CTable};
static s_RunParmPerI2C_t g_RunParmS[ZHL_I2C_Count]={0};

static void UH_I2C_InitReg(I2C_TypeDef * I2Cx,const s_com_Setting_t* const I2CSetting)
{
	LL_RCC_ClocksTypeDef rcc_clocks;
	LL_RCC_GetSystemClocksFreq(&rcc_clocks);
	unsigned int RegValue;
	if(LL_I2C_IsEnabled(I2Cx))
	{
		while((I2Cx->CR1 & (I2C_CR1_START | I2C_CR1_STOP | I2C_CR1_PEC))){};
		I2Cx->CR1=0;
	}
	const u_I2CTransFormat_t* TransMode=(const u_I2CTransFormat_t*)&I2CSetting->TransFormat;
	LL_I2C_ConfigSpeed(I2Cx,rcc_clocks.PCLK2_Frequency,I2CSetting->Baudrate,0);
	
	LL_I2C_EnableIT_EVT(I2Cx);
	LL_I2C_EnableIT_BUF(I2Cx);
	LL_I2C_EnableIT_ERR(I2Cx);
}

void UH_I2C_Init(int PortIndex,const s_com_Setting_t* Setting)
{
	void (*I2CInitFunTable[])(unsigned char)={_InitFunctionTable};
	void (*CallBack)(int,int);
	int id=PortIndex;
	I2C_TypeDef * I2Cx;
	switch(id)
	{
	case 0:
	case 1:
	case 2:
#if ZHL_Uart_Count>=4
	case 3:
#endif
		I2CInitFunTable[id](G_I2CGPIOGroupS[id]);
		I2Cx=g_I2CTable[id];
		
		break;

	default:
		return;
		break;
	}	
}


int UH_I2C_TransData(int PortIndex,const s_com_Setting_t* const I2CSetting,const char* Data,int Length)
{
	s_RunParmPerI2C_t* Parm=&g_RunParmS[PortIndex];
	if(*Data & 0x1)
	{
		Parm->RBuffer=(char*)Data;
		Parm->RWCount=Length+1;
		I2CPrintf("I2C开始接收数据\n");
	}
	else
	{
		Parm->TBuffer=Data;
		Parm->RWCount=Length;
		I2CPrintf("I2C开始发送数据\n");
	}
	Parm->RWCounter=0;
	I2C_TypeDef * I2Cx=g_I2CTable[PortIndex];
	UH_I2C_InitReg(I2Cx,I2CSetting);
	I2Cx->CR1 = I2C_CR1_PE | I2C_CR1_START | I2C_CR1_ACK; // send start
	return 0;
}

void UH_I2C_DeInit(int PortIndex)
{
	I2C_TypeDef * I2Cx=g_I2CTable[PortIndex];
	LL_I2C_DisableIT_EVT(I2Cx);
	LL_I2C_DisableIT_BUF(I2Cx);
	LL_I2C_DisableIT_ERR(I2Cx);
	if(LL_I2C_IsActiveFlag_BUSY(I2Cx))
	{
		LL_I2C_EnableReset(I2Cx);
	}
	I2Cx->CR1=0;
}

bool UH_I2C_IsOpen(int PortIndex)
{
	I2C_TypeDef * I2Cx=g_I2CTable[PortIndex];
	return I2Cx->CR1 & I2C_CR1_PE;
}

bool UH_I2C_IsBusy(int PortIndex)
{
	I2C_TypeDef * I2Cx=g_I2CTable[PortIndex];
	return I2Cx->SR2 & I2C_SR2_BUSY;
}

int UH_I2C_UpdateSetting(int PortIndex,const s_com_Setting_t* const NewPortConfig)
{
	I2C_TypeDef * I2Cx=g_I2CTable[PortIndex];
	UH_I2C_InitReg(I2Cx,NewPortConfig);
	return 0;
}

int UH_I2C_GetSetting(int PortIndex,s_com_Setting_t* GetTo)
{
	return 0;
}

static void UH_I2C_PacketSendHandler(int PortIndex)
{
	char* datap;
	int length;
	I2C_TypeDef * I2Cx=g_I2CTable[PortIndex];
	s_RunParmPerI2C_t* Parm=&g_RunParmS[PortIndex];
	
	
	e_I2CSendCallBackResult_t RT=UHC_I2C_PacketSended(PortIndex,&datap,&length);

	if (RT==I2C_SENDCBRT_NeedSending)
	{
		Parm->TBuffer=datap;
		Parm->RWCount=length;
		Parm->RWCounter=0;
	}
}

#define UH_I2C_DisableIT(__I2Cx,__IT)			ZHL_ClrBits(__I2Cx->CR2,__IT)

static void I2Cx_EV_IRQHandler(int PortIndex)
{
	I2C_TypeDef * I2Cx=g_I2CTable[PortIndex];
	s_RunParmPerI2C_t* Parm=&g_RunParmS[PortIndex];
    unsigned int sr1 = I2Cx->SR1;  // read status register
    unsigned int sr2 = I2Cx->SR2;  // clear ADDR bit
	
	if (sr1 & I2C_SR1_SB)
	{ // start bit
		if(*Parm->TBuffer & 0x1)
		{
			if(UH_I2C_IsReceiveOneByte(Parm))
			{
				ZHL_ClrBits(I2Cx->CR1,I2C_CR1_ACK);
			}
			else if(UH_I2C_IsReceiveTwoByte(Parm))
			{
				ZHL_SetBits(I2Cx->CR1,I2C_CR1_POS);
			}
		}
		I2Cx->DR = *Parm->TBuffer++; // send header byte with write bit;
		Parm->RWCounter++;
	}
	else
	{
		if(I2Cx->SR2 & I2C_SR2_TRA)	//发送数据
		{
            while (Parm->RWCounter<Parm->RWCount && (sr1 & I2C_SR1_TXE))
			{
               I2Cx->DR = *Parm->TBuffer++; // next data byte;
               sr1 = I2Cx->SR1;  // update status register copy
				if(++Parm->RWCounter>=Parm->RWCount)
				{
					if(Parm->RWCounter>=Parm->RWCount)
					{
						LL_I2C_DisableIT_BUF(I2Cx);
					}
					UH_I2C_PacketSendHandler(PortIndex);
				}

            }
            if ((sr1 & I2C_SR1_BTF))
			{
				I2Cx->CR1|=I2C_CR1_STOP;
				UH_I2C_DisableIT(I2Cx,I2C_CR2_ITERREN | I2C_CR2_ITEVTEN);
				UHC_I2C_DataSended(PortIndex);
				I2CPrintf("I2C发送完毕\n");
			} // last byte not yet sent
		}
		else
		{
            if (sr1 & I2C_SR1_ADDR) //地址发送完毕
			{ 
                if (UH_I2C_IsReceiveOneByte(Parm))
				{
                    I2Cx->CR1 |= I2C_CR1_STOP; // send stop after single byte
                }
				else if (UH_I2C_IsReceiveTwoByte(Parm))
				{
                    I2Cx->CR1 &= ~I2C_CR1_ACK; // last byte nack
                }
            } else {
                while (LL_I2C_IsActiveFlag_RXNE(I2Cx))
				{ 
					if (Parm->RWCount-Parm->RWCounter == 2)
					{ 
						if(LL_I2C_IsActiveFlag_BTF(I2Cx))
						{
							I2Cx->CR1 |= I2C_CR1_STOP; // stop after last byte
						}
						else 
						{
							if(LL_I2C_IsEnabledIT_BUF(I2Cx))
							{
								UH_I2C_DisableIT(I2Cx,I2C_CR2_ITBUFEN);
							}
							break;
						}
					}
					else if (Parm->RWCount-Parm->RWCounter == 3)
					{ 
						if (!LL_I2C_IsActiveFlag_BTF(I2Cx))
						{
							UH_I2C_DisableIT(I2Cx,I2C_CR2_ITBUFEN);
							break; // assure 2 bytes are received
						}
						I2Cx->CR1 &= ~I2C_CR1_ACK; // last byte nack
					}
					*Parm->RBuffer++ = I2Cx->DR; // read data
					Parm->RWCounter++;
                }
				if(Parm->RWCount<=Parm->RWCounter)
				{
					UHC_I2C_DataReceived(PortIndex,Parm->RWCounter);
					I2CPrintf("I2C接收完毕\n");
				}
            }
		}
	}
}

inline static unsigned int UH_I2C_ConvErrorFlag_HAL_TO_PAL(unsigned int HALFlag)
{
	return (HALFlag>>8) & I2C_ERR_Msk;
}

static void I2Cx_ER_IRQHandler(int PortIndex)
{
	I2C_TypeDef * I2Cx=g_I2CTable[PortIndex];
	s_RunParmPerI2C_t* Parm=&g_RunParmS[PortIndex];
	unsigned int ErrorFlag=I2Cx->SR1;
	UHC_I2C_Error(PortIndex,UH_I2C_ConvErrorFlag_HAL_TO_PAL(ErrorFlag));
	I2Cx->SR1 &= ~ErrorFlag;
}

void I2C1_EV_IRQHandler(void)
{
	I2Cx_EV_IRQHandler(0);
}

void I2C2_EV_IRQHandler(void)
{
	I2Cx_EV_IRQHandler(1);
}

void I2C1_ER_IRQHandler(void)
{
	I2Cx_ER_IRQHandler(0);
}

void I2C2_ER_IRQHandler(void)
{
	I2Cx_ER_IRQHandler(1);
}
