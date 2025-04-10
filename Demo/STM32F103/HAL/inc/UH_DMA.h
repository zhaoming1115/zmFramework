/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� UH_DMA.h 
�������ڣ�2025-04-09 
�ļ����ܣ���ӦC�ļ���ͷ�ļ�������C�ļ����⹫�����������͡��ӿں�����ȫ�ֱ�����

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

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
		unsigned int EN:1;		//ͨ��ʹ��
		unsigned int TCIEN:1;	//��������ж�ʹ��
		unsigned int HalfTCIEN:1;	//�봫������ж�ʹ��
		unsigned int ErrIEN:1;	//�쳣�ж�ʹ��
		unsigned int DTDir:1;	//0-����ΪԴ��1-�洢��ΪԴ
		unsigned int LoopModeEN:1;		//ѭ��ģʽʹ��
		unsigned int PICEN:1;		//�����ַ����ʹ��
		unsigned int MICEN:1;		//�洢����ַ����ʹ��
		unsigned int PDataWidth:2;		//0-8bit;1-16bit;2-32bit;3-����
		unsigned int MDataWidth:2;
		unsigned int YouXianJi:2;	//0~3-�ӵ͵���
		unsigned int M2M:1;			//�洢�����洢��ģʽ
		unsigned int REV:17;	//����

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
	����˵����	����DMA���䣬�˺�������DMA�Ĵ���λ�����ȼ������衢�жϵ��Ѿ�������ϣ�ֻ��Ҫ�����ڴ��ַ�ʹ���������������������
	����˵����	CHannelIndex-ͨ����������0��ʼ������DMAͳһ���
				MemoryAddr-�ڴ��ַ
				Length-�������������ֽ�������DMA����Ĵ���
*/
inline static void UH_DMA_ContinueTrans(int CHannelIndex,unsigned int MemoryAddr,unsigned int Length)
{
	DMA_Channel_TypeDef* DMACHannel=UH_DMA_GetCHannel(CHannelIndex);
	DMACHannel->CMAR=MemoryAddr;
	DMACHannel->CNDTR=Length;
	ZHL_SetBits(DMACHannel->CCR,DMA_CCR_EN);
}

/*
	����˵����	��ͣDMA����
	����˵����	CHannelIndex-ͨ����������0��ʼ������DMAͳһ���
				MemoryAddr-�ڴ��ַ
				Length-�������������ֽ�������DMA����Ĵ���
*/
inline static void UH_DMA_SuspendTrans(int CHannelIndex)
{
	DMA_Channel_TypeDef* DMACHannel=UH_DMA_GetCHannel(CHannelIndex);
	ZHL_ClrBits(DMACHannel->CCR,DMA_CCR_EN);
}

/*
	����˵����	��ȡDMAʣ�ഫ������
	����˵����	CHannelIndex-ͨ����������0��ʼ������DMAͳһ���
*/
inline static int UH_DMA_GetRemLength(int CHannelIndex)
{
	DMA_Channel_TypeDef* DMACHannel=UH_DMA_GetCHannel(CHannelIndex);
	return DMACHannel->CNDTR;
}

/*
	����˵����	��ȡDMAʣ�ഫ������
	����˵����	CHannelIndex-ͨ����������0��ʼ������DMAͳһ���
*/
inline static int UH_DMA_CHannelIsEnable(int CHannelIndex)
{
	DMA_Channel_TypeDef* DMACHannel=UH_DMA_GetCHannel(CHannelIndex);
	return ZHL_GetBits(DMACHannel->CCR,DMA_CCR_EN);
}


/*****************************************************************************
* �� �� ���� UH_DMA_CHannelIsCircle
* �������ܣ� ��ȡDMAͨ���Ƿ���ѭ��ģʽ
* ��    ���� CHannelIndex: [����] ��DMAͨ���ţ���0��ʼ���
* ��    �أ� ͨ����ѭ��ģʽλ��־	
* ��    ���� 

* �� �� �ߣ����� 
* ��    �ڣ� 2022-03-23

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
inline static int UH_DMA_CHannelIsCircle(int CHannelIndex)
{
	DMA_Channel_TypeDef* DMACHannel=UH_DMA_GetCHannel(CHannelIndex);
	return ZHL_GetBits(DMACHannel->CCR,DMA_CCR_CIRC );
}

#endif
