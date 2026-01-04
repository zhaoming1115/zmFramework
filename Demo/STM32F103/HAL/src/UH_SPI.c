/**
  ******************************************************************************
  * @file           : UH_SPI.c
  * @brief          : SPI硬件驱动程序
  ******************************************************************************
  * @attention
  *
  本程序基于意法半导体官方的LL库运行，是对LL库的再次整合，以适配其对应的平台抽象层程序。
	使用过程中如有任何问题，可随时与开发者联系。
	开发者姓名：赵明
	电子邮箱：zhaomingaoing@126.com
  *
  ******************************************************************************
  */
#include "HAL_Interface/HAL_Interface_SPI.h"
#include "..\HAL_Config.h"
#include "stm32f1xx_ll_spi.h"
#include "string.h"

extern __WEAK void MX_SPI1_Init(void);
extern __WEAK void MX_SPI2_Init(void);
extern __WEAK void MX_SPI3_Init(void);

__WEAK void MX_SPI1_Init(void)
{
	
}

__WEAK void MX_SPI2_Init(void)
{
}

__WEAK void MX_SPI3_Init(void)
{
}


#define _InitFunctionTable	MX_SPI1_Init,MX_SPI2_Init,MX_SPI3_Init

#ifdef SPI3
static const SPI_TypeDef* const g_spitalbe[]={SPI1,SPI2,SPI3};
#else
static const SPI_TypeDef* const g_spitalbe[]={SPI1,SPI2};
#endif 

#define UH_SPI_IsMaster(spi)			(LL_SPI_GetMode(spi)==LL_SPI_MODE_MASTER)

int UH_SPI_Init(int PortIndex,const s_com_Setting_t* Setting)
{
	static const unsigned char CPOLValueTable[]={0,SPI_CR1_CPOL};
	static const unsigned char CPHAValueTable[]={0,SPI_CR1_CPHA};
	void (*CallBack)(int,int);

	void (*SPIInitFunTable[])()={_InitFunctionTable};
	
	if(ZHL_SPI_IsValidPort(PortIndex))
	{
		SPI_TypeDef* spi=(SPI_TypeDef*)g_spitalbe[PortIndex];
		SPIInitFunTable[PortIndex]();
		UH_SPI_UpdateSetting(PortIndex,Setting);
	}	
	return 0;
}

void UH_SPI_DeInit(int PortIndex)
{
	if(ZHL_SPI_IsValidPort(PortIndex))
	{
		SPI_TypeDef* spi=(SPI_TypeDef*)g_spitalbe[PortIndex];
		LL_SPI_Disable(spi);
	}
}

inline static void UH_SPI_WaitAndClose(SPI_TypeDef* spi)
{
	if(LL_SPI_IsActiveFlag_BSY(spi))
	{
//		while(!LL_SPI_IsActiveFlag_RXNE(spi)){};
		while(!LL_SPI_IsActiveFlag_TXE(spi)){};
		while(LL_SPI_IsActiveFlag_BSY(spi)){};
	}
	(void)LL_SPI_ReceiveData8(spi);
	LL_SPI_Disable(spi);	
}

#define UH_SPI_WaitForReceiveData(spi) while ((LL_SPI_GetTransferDirection(spi)!=LL_SPI_HALF_DUPLEX_TX) && !LL_SPI_IsActiveFlag_RXNE(spi)){};

int UH_SPI_ReadWrite(int PortIndex,char* WRData,int Length)
{
	;
	if(!ZHL_SPI_IsValidPort(PortIndex))
	{
		return -1;
	}
	SPI_TypeDef* spi=(SPI_TypeDef*)g_spitalbe[PortIndex];
	LL_SPI_Enable(spi);			//使能标志同时也兼做忙指示

	if(LL_SPI_GetDataWidth(spi)==LL_SPI_DATAWIDTH_8BIT)
	{
		char* RxData=WRData;
		char* TxData=WRData;
		(void)LL_SPI_ReceiveData8(spi);
		while (LL_SPI_IsActiveFlag_TXE(spi)==0){};
		LL_SPI_TransmitData8(spi,*TxData++);
		for (int i=0;i<Length-1;i++)
		{
			while (LL_SPI_IsActiveFlag_TXE(spi)==0){};
			LL_SPI_TransmitData8(spi,*TxData++);
			UH_SPI_WaitForReceiveData(spi);
			*RxData++=LL_SPI_ReceiveData8(spi);	
		}
		while (LL_SPI_IsActiveFlag_TXE(spi)==0){};
		UH_SPI_WaitForReceiveData(spi);
		*RxData++=LL_SPI_ReceiveData8(spi);		
	}
	else
	{
		short* RxData=(short*)WRData;
		short* TxData=(short*)WRData;
	//	(void)LL_SPI_ReceiveData16(spi);
		while (LL_SPI_IsActiveFlag_TXE(spi)==0){};
		LL_SPI_TransmitData16(spi,*TxData++);
		for (int i=0;i<((Length+1)>>1)-1;i++)
		{
			while (LL_SPI_IsActiveFlag_TXE(spi)==0){};
			LL_SPI_TransmitData16(spi,*TxData++);
	//		UH_SPI_WaitForReceiveData(spi);
	//		*RxData++=LL_SPI_ReceiveData16(spi);	
		}
		while (LL_SPI_IsActiveFlag_TXE(spi)==0){};
	//	UH_SPI_WaitForReceiveData(spi);
	//	*RxData++=LL_SPI_ReceiveData16(spi);		
	}
	LL_SPI_Disable(spi);
	return Length;	
}

bool UH_SPI_IsBusy(int PortIndex)
{
	if(!ZHL_SPI_IsValidPort(PortIndex))
	{
		return false;
	}
	SPI_TypeDef* spi=(SPI_TypeDef*)g_spitalbe[PortIndex];
	return LL_SPI_IsEnabled(spi);
}

int UH_SPI_UpdateSetting(int PortIndex,const s_com_Setting_t* const NewPortConfig)
{
	if(!ZHL_SPI_IsValidPort(PortIndex))
	{
		return false;
	}
	SPI_TypeDef* spi=(SPI_TypeDef*)g_spitalbe[PortIndex];
	LL_SPI_Disable(spi);
	u_SPIFrameFormat_t* FrameFormat=(u_SPIFrameFormat_t*)&NewPortConfig->TransFormat;
	LL_SPI_SetClockPhase(spi,FrameFormat->CPHA<<SPI_CR1_CPHA_Pos);
	LL_SPI_SetClockPolarity(spi,FrameFormat->CPOL<<SPI_CR1_CPOL_Pos);
	LL_SPI_SetMode(spi,FrameFormat->MasterFlag<<SPI_CR1_MSTR_Pos);
	LL_SPI_SetDataWidth(spi,FrameFormat->DataUseBit16<<SPI_CR1_DFF_Pos);
	MODIFY_REG(spi->CR1, SPI_CR1_SSM,  FrameFormat->UseNSS<<SPI_CR1_SSM_Pos);
	if(FrameFormat->MasterFlag)
	{
  		MODIFY_REG(spi->CR2, SPI_CR2_SSOE, 1);
	}
	else
	{
  		MODIFY_REG(spi->CR2, SPI_CR2_SSOE, 0);
	}
	LL_SPI_SetClockPhase(spi,FrameFormat->CPHA<<SPI_CR1_CPHA_Pos);
	LL_RCC_ClocksTypeDef Clocks;
	LL_RCC_GetSystemClocksFreq(&Clocks);
	unsigned int F=(PortIndex==0)?Clocks.PCLK2_Frequency:Clocks.PCLK1_Frequency;
	unsigned int YinZi=F/NewPortConfig->Baudrate;
	unsigned int Prescaler=0;
	YinZi>>=1;
	while(YinZi>1)
	{
		Prescaler++;
		YinZi>>=1;
	}
	LL_SPI_SetBaudRatePrescaler(spi,Prescaler<<SPI_CR1_BR_Pos);
	return 0;
}

int UH_SPI_GetSetting(int PortIndex,s_com_Setting_t* GetTo)
{
	if(!ZHL_SPI_IsValidPort(PortIndex))
	{
		return false;
	}
	SPI_TypeDef* spi=(SPI_TypeDef*)g_spitalbe[PortIndex];
	u_SPIFrameFormat_t FrameFormat={0};
	FrameFormat.CPHA=LL_SPI_GetClockPhase(spi)>>SPI_CR1_CPHA_Pos;
	FrameFormat.CPOL=LL_SPI_GetClockPolarity(spi)>>SPI_CR1_CPOL_Pos;
	FrameFormat.MasterFlag=LL_SPI_GetMode(spi)>>SPI_CR1_MSTR_Pos;
	FrameFormat.DataUseBit16=LL_SPI_GetDataWidth(spi)>>SPI_CR1_DFF_Pos;
	FrameFormat.UseNSS=READ_BIT(spi->CR1, SPI_CR1_SSM)>>SPI_CR1_SSM_Pos;
	GetTo->TransFormat=FrameFormat.FrameFormat;
	LL_RCC_ClocksTypeDef Clocks;
	LL_RCC_GetSystemClocksFreq(&Clocks);
	unsigned int F=(PortIndex==0)?Clocks.PCLK2_Frequency:Clocks.PCLK1_Frequency;
	GetTo->Baudrate= F>>((LL_SPI_GetBaudRatePrescaler(spi)>>SPI_CR1_BR_Pos)+1);
	return 0;
}




