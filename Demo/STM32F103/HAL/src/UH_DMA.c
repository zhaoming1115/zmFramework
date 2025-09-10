/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： UH_DMA.c 
开发日期：2025-04-09 
文件功能：

描    述：

依赖接口：

提供接口：
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/
#include "..\HAL_Config.h"
#include "..\Inc\UH_DMA.h"

#pragma pack(4)
typedef struct
{
	void (*CallBack)(int CHannelID,int Flag);
}JG_DMARunParmLX;

#pragma pack()

static JG_DMARunParmLX g_RunParm[ZHL_DMA_CHannelCount];

void UH_DMA_InitCHannel(int CHannelIndex,const s_DMACHannelInitParm_t* InitParm,unsigned int PeriphAddr,void (*CallBack)(int CHannelID,int Flag))
{
	DMA_TypeDef* DMA=UH_DMA_GetDMA(CHannelIndex);
	DMA_Channel_TypeDef* DMACHannel=UH_DMA_GetCHannel(CHannelIndex);
	int IRQn;
	if(DMA==DMA1)
	{
		IRQn=DMA1_Channel1_IRQn+CHannelIndex;
		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
	}
	else
	{
		int InnerID=CHannelIndex-ZHL_DMA_CHannelsPerDMA;
		IRQn=(InnerID==4)?DMA2_Channel1_IRQn+3:DMA2_Channel1_IRQn+InnerID;
		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
	}
	DMA->IFCR&=~0xf<<((CHannelIndex % ZHL_DMA_CHannelsPerDMA)<<2);	//清除所有该通道中断
	DMACHannel->CCR=InitParm->CTRLValue;
	DMACHannel->CPAR=PeriphAddr;
	g_RunParm[CHannelIndex].CallBack=CallBack;
	
	NVIC_SetPriority(IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),InitParm->IRQPriority, 0));
	NVIC_EnableIRQ(IRQn);
}

inline static void UH_DMA_OnEvent(DMA_TypeDef* DMA,int CHannelID)
{
	int _bits=(CHannelID % ZHL_DMA_CHannelsPerDMA)<<2;
	int Flag=DMA->ISR & (0xf<<_bits);
	if (Flag==0) return;	//如果没有中断标志，则直接退出
	DMA->IFCR|=Flag;

	if (g_RunParm[CHannelID].CallBack)
	{
		g_RunParm[CHannelID].CallBack(CHannelID,Flag>>_bits);
	}
}
/**
  * @brief This function handles DMA1 channel2 global interrupt.
  */
void DMA1_Channel2_IRQHandler(void)
{
	UH_DMA_OnEvent(DMA1,1);
}

/**
  * @brief This function handles DMA1 channel3 global interrupt.
  */
void DMA1_Channel3_IRQHandler(void)
{
	UH_DMA_OnEvent(DMA1,2);

}

/**
  * @brief This function handles DMA1 channel4 global interrupt.
  */
void DMA1_Channel4_IRQHandler(void)
{
	UH_DMA_OnEvent(DMA1,3);
}

/**
  * @brief This function handles DMA1 channel5 global interrupt.
  */
void DMA1_Channel5_IRQHandler(void)
{
	UH_DMA_OnEvent(DMA1,4);
}

/**
  * @brief This function handles DMA1 channel6 global interrupt.
  */
void DMA1_Channel6_IRQHandler(void)
{
	UH_DMA_OnEvent(DMA1,5);

}

/**
  * @brief This function handles DMA1 channel7 global interrupt.
  */
void DMA1_Channel7_IRQHandler(void)
{
	UH_DMA_OnEvent(DMA1,6);
}

#ifdef DMA2
/**
  * @brief This function handles DMA2 channel1 global interrupt.
  */
void DMA2_Channel1_IRQHandler(void)
{
	UH_DMA_OnEvent(DMA2,7);
}

/**
  * @brief This function handles DMA2 channel2 global interrupt.
  */
void DMA2_Channel2_IRQHandler(void)
{
	UH_DMA_OnEvent(DMA2,8);
}

/**
  * @brief This function handles DMA2 channel3 global interrupt.
  */
void DMA2_Channel3_IRQHandler(void)
{
	UH_DMA_OnEvent(DMA2,9);
}

/**
  * @brief This function handles DMA2 channel4 and channel5 global interrupts.
  */
void DMA2_Channel4_5_IRQHandler(void)
{
	UH_DMA_OnEvent(DMA2,10);
	UH_DMA_OnEvent(DMA2,11);
}
#endif