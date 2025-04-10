/*****************************************************************************
 *   Copyright(C)2009-2019 by GorgonMeducer<embedded_zhuoran@hotmail.com>    *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/
/*���ļ�����PLOOC���ƣ�������ԭ��˵��*/
/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� zmQueue.h 
�������ڣ�2025-04-09 
�ļ����ܣ���ӦC�ļ���ͷ�ļ�������C�ļ����⹫�����������͡��ӿں�����ȫ�ֱ�����

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/

 #ifndef __ZM_QUEUE_H_
#define __ZM_QUEUE_H_

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../Sys_DataType.h"
#include "../Sys_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__Queue_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__Queue_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"

#define	Queue_DataLength_OneFrame			0xffff	//��ֵ����һ֡���ݳ��ȣ�����Ԫ�ش�СΪ1ʱ������ʵ����Ҫ�������ֽ���Ϊ65535����������û��ô����ᰴ�ջ�������С������
#define Queue_UseFullAndEmptyAlarm			1		//ʹ�ö������澯

#define __Queue_ElementSizeUsePower2		1		//0-Ԫ�ش�Сʹ����ʵ�Ĵ�С��1-Ԫ�ش�Сʹ�û�2��ָ��
typedef enum 
{
	Queue_OPRST_Base=-16,	//
	Queue_OPRST_Fulled,		//����������������֡�������ڵ��ڶ����п����ɵ�֡����
	Queue_OPRST_Emptyed,		//���пգ���ʱ����ҲӦ��Ϊ��
	Queue_OPRST_ThisFrameIsPart,		//���ֻ֡��һ���֣����쳣����������·��أ�1-��������ӵ�����С��һ֡������ʱ��2-������ӵ���������0�Ҹպ���Ԫ�ش�С��������
	Queue_OPRST_NoEnoughBuffer,		//û���㹻�Ļ��棬�����Ԫ���������ڵ��ڶ���ʣ��ռ�ʱ�ᴥ�����쳣��
//	Queue_RST_OKAndPart=-2,			//������������һ����
	Queue_RST_OK=0,					//����
	//����0��������
}e_QueueOperationResult_t;

typedef enum 
{
	Queue_RegionState_Empty,
	Queue_RegionState_HaveData,
	Queue_RegionState_Full,
}e_QueueRegionState_t;

typedef union
{
	unsigned char Value;
	struct
	{
		unsigned char PartFrameFlag:1;
#if Queue_UseFullAndEmptyAlarm
		unsigned char FullFlag:1;
		unsigned char EmptyFlag:1;
		unsigned char BaoLiu:5;
#else
		unsigned char BaoLiu:7;

#endif
	};
}u_QueueAlarmState_t;

typedef union
{
	unsigned short Value;
	struct
	{
		u_QueueAlarmState_t AlarmState;
		unsigned short UserState;
	};
}u_QueueState_t;

//! \name class Queue_t
//! @{
declare_class(c_Queue_t)
declare_interface(i_Queue_t)

#if __Queue_ElementSizeUsePower2

typedef struct Queue_cfg_t {
    
    //! put your configuration members here
	unsigned char ElementSizeLevel;		//Ԫ�ش�С�ȼ�����2
	unsigned char UserData;		//�û�����
    unsigned short ElementCount;	//Ԫ������
} s_Queue_cfg_t;

/*���һ����������
__ElementSizeLevel��Ԫ�ش�С�ȼ�����2
__ElementCount��Ԫ������
*/
#define Append_QueueConfig(__ElementSizeLevel,__ElementCount)		{.ElementSizeLevel=__ElementSizeLevel,.ElementCount=__ElementCount}

#else
typedef struct Queue_cfg_t {   
	unsigned short ElementSize;		//Ԫ�ش�С
    unsigned short ElementCount;	//Ԫ������
} s_Queue_cfg_t;
#define Append_QueueConfig(__ElementSize,__ElementCount)		{.ElementSize=__ElementSize,.ElementCount=__ElementCount}

#endif


def_class(c_Queue_t,

    public_member(
        //!< place your public members here
		const i_Queue_t* Manager;
    )

    private_member(
        //!< place your private members here
    )
    
    protected_member(
        //!< place your private members here
		const s_Queue_cfg_t Config;
		unsigned short ReadIndex;
		unsigned short WriteIndex;
		unsigned short ElementCount;
		u_QueueState_t State;
    )
)

end_def_class(c_Queue_t) /* do not remove this for forward compatibility  */
//! @}



//! \name interface i_Queue_t
//! @{
def_interface(i_Queue_t)
    c_Queue_t *  (*Create) (const s_Queue_cfg_t *ptCFG);
    void         (*Dispose)(c_Queue_t *ptObj);
/*****************************************************************************
* �� �� ���� Enqueue
* �������ܣ� ���һ������Ԫ��
* ��    ���� Queue: ���ж��� 
**			 Data: [����]��Ҫ��ӵ�������,��Ϊ�գ�����������
**			 DataLength: Ҫ��ӵ����������� 
* ��    �أ� ���ɹ�������ʵ����ӵ����ݳ��ȣ����򷵻��쳣�룬�쳣���e_QueueOperationResult_t
* ��    ���� ��ӵ���������С��Ԫ�ش�С����ʱռ��һ��Ԫ�صĿռ䣻
			 Ҳ�ɴ���Ԫ�ش�С����ʱռ�ö��Ԫ�ؿռ䣬������ȡ����
			��������û���㹻�Ŀռ䣬�򷵻�Queue_OPRST_NoEnoughBuffer

* �� �� �ߣ����� 
* ��    �ڣ� 2024-03-24

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	int (*Enqueue)(c_Queue_t* Queue,__In char* Data,unsigned int DataLength);
	
/*****************************************************************************
* �� �� ���� Dequeue
* �������ܣ� ����һ������
* ��    ���� Queue: ���ж��� 
**			 Data: ����ȥ����Ϊ�գ��򲻸������� 
**			 DataLength: Ҫ���ӵ���������ֽ���
* ��    �أ� ���ɹ���ʵ�ʳ��ӵ������ֽ��������򷵻��쳣�룬�쳣���e_QueueOperationResult_t
* ��    ���� ��DataLength���ڵ��ڶ����е�һ��֡���ݳ���ʱ������һ֡���ݣ�������ʵ�ʳ��ӵ��ֽ�����
			 ��DataLengthС�ڶ����е�һ��֡���ݳ���ʱ�����ӵ�������ΪL,L=DataLength-DataLength % ElementSize,������Queue_OPRST_DstBufferOverflow��

* �� �� �ߣ����� 
* ��    �ڣ� 2024-03-24

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	int (*Dequeue)(c_Queue_t* Queue,__Out char* Data,unsigned int DataLength);
	
/*****************************************************************************
* �� �� ���� AskEnqueue
* �������ܣ� �������
* ��    ���� Queue: ���ж��� 
**			 DataBuffer: [���]�����뵽�Ļ����ַ
* ��    �أ� ���ɹ����������뵽�Ļ��泤�ȣ��ó���ΪElementSize��������
* ��    ���� ������������Ϊ����DMA�ṩ����ռ䣬��DMA�����������Ҫ����Enqueue������������ʽ��ӣ������������Ҫ��Enqueue�е�Data��������Ϊ��

* �� �� �ߣ����� 
* ��    �ڣ� 2024-03-24

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	int (*AskEnqueue)(c_Queue_t* Queue,__Out char** DataBuffer);

/*****************************************************************************
* �� �� ���� int
* �������ܣ� �������
* ��    ���� Queue: ���ж���
**			 DataBuffer: [���]�����׵����ݻ����ַ
* ��    �أ����ɹ������ر��οɳ��ӵ����ݳ��ȣ��ó���Ϊһ֡���ݵ�ʵ�ʳ��ȣ�������ElementSize��������
* ��    ����������������Ϊ����DMA�ṩ����Դ���䣬��DMA�����������Ҫ����Dequeue������������ʽ���ӣ������������Ҫ��Dequeue�е�Data��������Ϊ��

* �� �� �ߣ����� 
* ��    �ڣ� 2024-03-24

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	int (*AskDequeue)(c_Queue_t* Queue,__Out const char** DataBuffer);

	int (*ByteCount)(c_Queue_t* Queue);
	int (*RemBufferSize)(c_Queue_t* Queue);
	int (*Clear)(c_Queue_t* Queue);
	u_QueueState_t (*GetState)(c_Queue_t* Queue);
    /* other methods */

end_def_interface(i_Queue_t) /*do not remove this for forward compatibility */
//! @}

#if __Queue_ElementSizeUsePower2
#define Queue_GetElementSize()			(1<<cfg->ElementSizeLevel)
inline static unsigned int Queue_remElementSize(__In s_Queue_cfg_t* Config,unsigned int Size)
{
	return Size & ((1<<Config->ElementSizeLevel)-1);
}

inline static unsigned int Queue_AlignToElementSize(__In s_Queue_cfg_t* Config,unsigned int Size,__Out unsigned int* ElementCount)
{
	*ElementCount=Size>>Config->ElementSizeLevel;
	return Size & ~((1<<Config->ElementSizeLevel)-1);
}

#else
#define Queue_GetElementSize()		cfg->ElementSize
inline static unsigned int Queue_remElementSize(__In s_Queue_cfg_t* Config,unsigned int Size)
{
	return Size % Config->ElementSize;
}

inline static unsigned int Queue_AlignToElementSize(__In s_Queue_cfg_t* Config,unsigned int Size,__Out unsigned int* ElementCount)
{
	div_t  result=div(Size,Config->ElementSize);
	*ElementCount=result.quot
	return Size - result.rem;
}

#endif


/*============================ GLOBAL VARIABLES ==============================*/

/*============================ PROTOTYPES ====================================*/

#if defined(__Queue_CLASS_IMPLEMENT__) || defined(__Queue_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void Queue_protected_method_example(c_Queue_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __Queue_CLASS_INHERIT__
#undef __Queue_CLASS_IMPLEMENT__

#endif
/* EOF */
