/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： UH_DMA.h 
开发日期：2025-04-09 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __U_H__D_M_A_H_
#define __U_H__D_M_A_H_
#include "..\HAL_Config.h"
#include "stm32f1xx_ll_dma.h"

#pragma pack(4)
//typedef struct
//{
//	void (*DMATransReport)(int Flag);
//}JG_DMAEventLX;

typedef union
{
	unsigned int CTRLValue;
	struct
	{
		unsigned int EN:1;		//通道使能
		unsigned int TCIEN:1;	//传输完成中断使能
		unsigned int HalfTCIEN:1;	//半传输完成中断使能
		unsigned int ErrIEN:1;	//异常中断使能
		unsigned int DTDir:1;	//0-外设为源；1-存储器为源
		unsigned int LoopModeEN:1;		//循环模式使能
		unsigned int PICEN:1;		//外设地址递增使能
		unsigned int MICEN:1;		//存储器地址递增使能
		unsigned int PDataWidth:2;		//0-8bit;1-16bit;2-32bit;3-保留
		unsigned int MDataWidth:2;
		unsigned int YouXianJi:2;	//0~3-从低到高
		unsigned int M2M:1;			//存储器到存储器模式
		unsigned int REV:17;	//保留

	};
}LH_DMACHannelCtrlLX;
#pragma pack()

inline static DMA_TypeDef* UH_DMA_GetDMA(int CHannelID)
{
#ifdef DMA2
	return (CHannelID<ZHL_DMA_CHannelsPerDMA)?DMA1:DMA2;
#else
	return DMA1;
#endif
}

inline static DMA_Channel_TypeDef* UH_DMA_GetCHannel(int CHannelID)
{
#ifdef DMA2
	unsigned int Addr=(CHannelID<ZHL_DMA_CHannelsPerDMA)? DMA1_Channel1_BASE:DMA2_Channel1_BASE;
#else
	unsigned int Addr= DMA1_Channel1_BASE;
#endif
	Addr+=+(CHannelID%ZHL_DMA_CHannelsPerDMA)*0x14;
	return (DMA_Channel_TypeDef*)Addr;
}

void UH_DMA_InitCHannel(int CHannelIndex,const LH_DMACHannelCtrlLX* Ctrl,unsigned int PeriphAddr,void (*CallBack)(int CHannelID,int Flag));

inline static void UH_DMA_EnableIT(int CHannelIndex,int ITFlag)
{
	DMA_Channel_TypeDef* DMACHannel=UH_DMA_GetCHannel(CHannelIndex);
	ZHL_SetBits(DMACHannel->CCR,ITFlag);
}

/*
	函数说明：	继续DMA传输，此函数基于DMA的传输位宽、优先级、外设、中断等已经配置完毕，只需要配置内存地址和传输量即可启动传输的情况
	参数说明：	CHannelIndex-通道索引，从0开始，所有DMA统一编号
				MemoryAddr-内存地址
				Length-传输量，不是字节量，是DMA传输的次数
*/
inline static void UH_DMA_ContinueTrans(int CHannelIndex,unsigned int MemoryAddr,unsigned int Length)
{
	DMA_Channel_TypeDef* DMACHannel=UH_DMA_GetCHannel(CHannelIndex);
	DMACHannel->CMAR=MemoryAddr;
	DMACHannel->CNDTR=Length;
	ZHL_SetBits(DMACHannel->CCR,DMA_CCR_EN);
}

/*
	函数说明：	暂停DMA传输
	参数说明：	CHannelIndex-通道索引，从0开始，所有DMA统一编号
				MemoryAddr-内存地址
				Length-传输量，不是字节量，是DMA传输的次数
*/
inline static void UH_DMA_SuspendTrans(int CHannelIndex)
{
	DMA_Channel_TypeDef* DMACHannel=UH_DMA_GetCHannel(CHannelIndex);
	ZHL_ClrBits(DMACHannel->CCR,DMA_CCR_EN);
}

/*
	函数说明：	获取DMA剩余传输数量
	参数说明：	CHannelIndex-通道索引，从0开始，所有DMA统一编号
*/
inline static int UH_DMA_GetRemLength(int CHannelIndex)
{
	DMA_Channel_TypeDef* DMACHannel=UH_DMA_GetCHannel(CHannelIndex);
	return DMACHannel->CNDTR;
}

/*
	函数说明：	获取DMA剩余传输数量
	参数说明：	CHannelIndex-通道索引，从0开始，所有DMA统一编号
*/
inline static int UH_DMA_CHannelIsEnable(int CHannelIndex)
{
	DMA_Channel_TypeDef* DMACHannel=UH_DMA_GetCHannel(CHannelIndex);
	return ZHL_GetBits(DMACHannel->CCR,DMA_CCR_EN);
}


/*****************************************************************************
* 函 数 名： UH_DMA_CHannelIsCircle
* 函数功能： 获取DMA通道是否是循环模式
* 参    数： CHannelIndex: [输入] ，DMA通道号，从0开始编号
* 返    回： 通道的循环模式位标志	
* 描    述： 

* 开 发 者：赵明 
* 日    期： 2022-03-23

* 修改历史：
** 1. 新创建

 *****************************************************************************/
inline static int UH_DMA_CHannelIsCircle(int CHannelIndex)
{
	DMA_Channel_TypeDef* DMACHannel=UH_DMA_GetCHannel(CHannelIndex);
	return ZHL_GetBits(DMACHannel->CCR,DMA_CCR_CIRC );
}

#endif
