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
/*���ļ�����PLOOC������������ԭ��˵��*/
/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� zmFrameQueue.c 
�������ڣ�2025-03-28 
�ļ����ܣ�ʵ��֡���ݵĶ���ʽ����

��    �������������һ�������Ļ�����������λ������ָ�Ϊn����Ԫ��ÿ����Ԫ��Ϊ1��Ԫ�أ���ʵ�ֶԲ�����֡���ݵĶ���ʽ����
			Ԫ�ش�С��СΪ1���ֽڣ�֡���ȿ��Դ��ڡ�С�ڻ����Ԫ�ش�С��������Ԫ�ش�Сʱ����������Ժ���1��Ԫ��Ϊ֡β��־����ʱ�ռ���������͡�
		�û���ͨ���ڴ������ж���ʱָ��Ԫ�ش�С��Ԫ�ظ������ڿռ������ʺ�ִ��Ч�ʷ���Ѱ�Ҿ��⡣

�����ӿڣ�

�ṩ�ӿڣ�zmFrameQueueManager����
	

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/


/*============================ INCLUDES ======================================*/

#define __Queue_CLASS_INHERIT__
#define __FrameQueue_CLASS_IMPLEMENT__

#include "zmFrameQueue.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)    assert(__VA_ARGS__)
#endif

#undef this
#define this        ptThis

#undef base
#define base        ptBase

#define Object_cfg(Object,cfgname)		const s_Queue_cfg_t* cfgname=&Object->Config

/*============================ TYPES =========================================*/
typedef struct 
{
	unsigned short ByteCount;
}s_ElementsHeader_t;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

/*============================ IMPLEMENTATION ================================*/

/*! \brief the constructor of the class: Queue */

#if __Queue_ElementSizeUsePower2
#define FrameQueue_GetElementSize()			(1<<cfg->ElementSizeLevel)
inline static unsigned int FrameQueue_remElementSize(__In s_Queue_cfg_t* Config,unsigned int Size)
{
	return Size & ((1<<Config->ElementSizeLevel)-1);
}

inline static unsigned int FrameQueue_AlignToElementSize(__In s_Queue_cfg_t* Config,unsigned int Size,__Out unsigned int* ElementCount)
{
	*ElementCount=Size>>Config->ElementSizeLevel;
	return Size & ~((1<<Config->ElementSizeLevel)-1);
}

#else
#define FrameQueue_GetElementSize()		cfg->ElementSize
inline static unsigned int FrameQueue_remElementSize(__In s_Queue_cfg_t* Config,unsigned int Size)
{
	return Size % Config->ElementCount;
}

inline static unsigned int FrameQueue_AlignToElementSize(__In s_Queue_cfg_t* Config,unsigned int Size,__Out unsigned int* ElementCount)
{
	div_t  result=div(Size,Config->ElementSize);
	*ElementCount=result.quot
	return Size - result.rem;
}

#endif

static
c_Queue_t *  FrameQueue_Create (const s_Queue_cfg_t *Config)
{
	const s_Queue_cfg_t* cfg=Config;
	unsigned int ElementSize=FrameQueue_GetElementSize();
	unsigned int bufsize=ElementSize*(unsigned int)Config->ElementCount;
	unsigned int ElementInfoSize=(ElementSize>1)?(ElementSize*sizeof(s_ElementsHeader_t)+3)&~3:0;

	unsigned int length=sizeof(c_Queue_t)+bufsize+ElementInfoSize;
	char* _buf=(char*)Sys_Malloc(length);
	if (_buf)
	{
		memset(_buf,0,length);
	}
	else
	{
		return NULL;
	}
	_buf+=ElementInfoSize;
	c_Queue_t* _queue=(c_Queue_t*)(void*)_buf;
	_buf+=sizeof(c_Queue_t);
	s_Queue_cfg_t* tocfg=(s_Queue_cfg_t*)(void*)&_queue->Config;
	memcpy(tocfg,cfg,sizeof(*cfg));
	_queue->Manager=&zmFrameQueueManager->use_as__i_Queue_t;
	return _queue;
}

static
void   FrameQueue_Dispose(c_Queue_t *Queue)
{
	if(Queue)
	{
		Object_cfg(Queue,cfg);
		unsigned int ElementSize=FrameQueue_GetElementSize();
		unsigned int ElementInfoSize=(ElementSize>1)?(ElementSize*sizeof(s_ElementsHeader_t)+3)&~3:0;
		char* buf=(char*)(void*)Queue;
		buf-=ElementInfoSize;
		Sys_Free(buf);
	}
}

inline static char* FrameQueue_GetBufferDataBase(c_Queue_t* Queue)
{
	return (char*)((unsigned int)Queue+sizeof(c_Queue_t));
}

inline static s_ElementsHeader_t* FrameQueue_GetElementHeader(c_Queue_t* Queue,typeof(Queue->ReadIndex) ElementIndex)
{
	s_ElementsHeader_t* BaseHeader=(s_ElementsHeader_t*)Queue;
	BaseHeader-=ElementIndex+1;
	return BaseHeader;
}

inline static s_ElementsHeader_t* FrameQueue_GetReadElementHeader(c_Queue_t* Queue)
{
	return FrameQueue_GetElementHeader(Queue,Queue->ReadIndex);
}

#define FrameQueue_ElementsHeaderToNext(ElementsHeader)		ElementsHeader--

static int FrameQueue_UpdateElementHeader(c_Queue_t* Queue,unsigned int DataLength)
{
	Object_cfg(Queue,cfg);
	s_ElementsHeader_t* Header=(s_ElementsHeader_t*)Queue;
	s_ElementsHeader_t* EndHeader=Header-cfg->ElementCount;

	Header-=Queue->WriteIndex+1;
	
	unsigned int ElementSize=FrameQueue_GetElementSize();
	unsigned int Size=ElementSize;
	unsigned int eCount=0;
	do
	{
		if(Size>DataLength) Size=DataLength;
		Header->ByteCount=Size;
		Header=(Header==EndHeader)?(Header+cfg->ElementCount-1):Header-1;	
		DataLength-=Size;
		eCount++;
	}while(DataLength>0);
	return eCount;
}

static void  FrameQueue_IncReadIndex(c_Queue_t* Queue,unsigned int AddValue)
{
	Object_cfg(Queue,cfg);
	
	Queue_SafeCode({
		Queue->ElementCount-=AddValue;
		typeof(Queue->ReadIndex) tmp=Queue->ReadIndex+AddValue;
		Queue->ReadIndex=(tmp>=cfg->ElementCount)?tmp-cfg->ElementCount:tmp;
#if Queue_UseFullAndEmptyAlarm
		Queue->State.AlarmState.EmptyFlag=(Queue->ElementCount<=0);
		Queue->State.AlarmState.FullFlag=0;
#endif
	});
}

static void  FrameQueue_IncWriteIndex(c_Queue_t* Queue,unsigned int AddValue)
{
	Object_cfg(Queue,cfg);
	Queue_SafeCode{
		typeof(Queue->WriteIndex) tmp=Queue->WriteIndex+AddValue;
		Queue->WriteIndex=(tmp>=cfg->ElementCount)?tmp-cfg->ElementCount:tmp;
		Queue->ElementCount+=AddValue;
#if Queue_UseFullAndEmptyAlarm
		Queue->State.AlarmState.FullFlag=(Queue->ElementCount>=cfg->ElementCount);
		Queue->State.AlarmState.EmptyFlag=0;
#endif
	}
}

static
int FrameQueue_ByteCount(c_Queue_t* Queue)
{
	Object_cfg(Queue,cfg);
	unsigned int ElementSize=FrameQueue_GetElementSize();
	
	return ElementSize*Queue->ElementCount;
}

static
int FrameQueue_RemBufferSize(c_Queue_t* Queue)
{
	Object_cfg(Queue,cfg);
	unsigned int ElementSize=FrameQueue_GetElementSize();
	return ElementSize*(cfg->ElementCount- Queue->ElementCount);	
}

static
int FrameQueue_AskDeQueue_Inner(c_Queue_t* Queue,__Out const char** DataBuffer,unsigned int* CanDeCount)
{
	if (Queue->ElementCount <=0) return Queue_OPRST_Emptyed;
	char* wrp=FrameQueue_GetBufferDataBase(Queue);
	Object_cfg(Queue,cfg);
	unsigned int ElementSize=FrameQueue_GetElementSize();
	int Size=0;
	if(ElementSize==1)
	{
		*DataBuffer=wrp+Queue->ReadIndex;
		Size=(Queue->WriteIndex<Queue->ReadIndex)?cfg->ElementCount-Queue->ReadIndex:Queue->WriteIndex-Queue->ReadIndex;
		*CanDeCount=Size;
		return Size;
	}
	else
	{
		*DataBuffer=wrp+Queue->ReadIndex*ElementSize;
		int Size=0;
		int id=Queue->ReadIndex;
		
		int endid=(Queue->WriteIndex>Queue->ReadIndex)?Queue->WriteIndex:cfg->ElementCount;
		while(id<endid)
		{
			s_ElementsHeader_t* Header=FrameQueue_GetElementHeader(Queue,id);
			id++;
			Size+=Header->ByteCount;
			if(Header->ByteCount<ElementSize)
			{
				break;
			}
		}
		*CanDeCount=id-Queue->ReadIndex;
		return Size;
	}
}

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
static
int FrameQueue_AskDequeue(c_Queue_t* Queue,__Out const char** DataBuffer)
{
	unsigned int eCount;
	return FrameQueue_AskDeQueue_Inner(Queue,DataBuffer,&eCount);
}

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
static
int FrameQueue_Dequeue(c_Queue_t* Queue,__Out char* Data,unsigned int DataLength)
{
	const char* srcdata;
	int remlen=DataLength;
	int rst=0;
	unsigned int TeCount=0;
	Object_cfg(Queue,cfg);
	unsigned int ElementSize=FrameQueue_GetElementSize();

	do
	{
		unsigned int eCount=0;
		int len=FrameQueue_AskDeQueue_Inner(Queue,&srcdata,&eCount);
//		if(len<0) return rst; 
		if(eCount==0) break;
		if(len>remlen)
		{
			remlen=FrameQueue_AlignToElementSize(cfg, remlen,&eCount);
			len=remlen;
		}
		rst+=len;
		if(Data)
		{
			memcpy(Data,srcdata,len);
			Data+=len;
		}

		FrameQueue_IncReadIndex(Queue,eCount);
		TeCount+=eCount;
		remlen-=len;	
		if(ElementSize * eCount>len) break;
	}while(remlen>0);
	if(Queue->ElementCount>0)
	{
		s_ElementsHeader_t* Header=FrameQueue_GetReadElementHeader(Queue);
		if(Header->ByteCount==0) 
		{
			FrameQueue_IncReadIndex(Queue,1);
			TeCount++;
		}
	}
	if(ElementSize>1 && ElementSize * TeCount == rst)
	{
		Queue->State.AlarmState.PartFrameFlag=1;
	}
	else
	{
		Queue->State.AlarmState.PartFrameFlag=0;
	}
	return rst;
}
	
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
static
int FrameQueue_AskEnqueue(c_Queue_t* Queue,__Out char** DataBuffer)
{
	
	Object_cfg(Queue,cfg);
	if (Queue->ElementCount>=cfg->ElementCount)
	{
		return Queue_OPRST_Fulled;
	}
	unsigned int eCount=(Queue->WriteIndex>=Queue->ReadIndex)?cfg->ElementCount-Queue->WriteIndex:Queue->ReadIndex-Queue->WriteIndex;
	unsigned int esize=FrameQueue_GetElementSize();
	char* wrp=FrameQueue_GetBufferDataBase(Queue);	
	*DataBuffer=wrp+Queue->WriteIndex*esize;	
	return esize*eCount;
}

/*****************************************************************************
* �� �� ���� Enqueue
* �������ܣ� ���һ������Ԫ��
* ��    ���� Queue: ���ж��� 
**			 Data: [����]��Ҫ��ӵ�������,��Ϊ�գ�����������
**			 DataLength: Ҫ��ӵ����������� 
* ��    �أ� ���ɹ�������ʵ����ӵ����ݳ��ȣ����򷵻��쳣�룬�쳣���e_QueueOperationResult_t
* ��    ���� ��ӵ���������С��Ԫ�ش�С����ʱռ��һ��Ԫ�صĿռ䣻
			 Ҳ�ɴ���Ԫ�ش�С����ʱռ�ö��Ԫ�ؿռ䣬������ȡ����
			�������������Ḳ�ǵ���������

* �� �� �ߣ����� 
* ��    �ڣ� 2024-03-24

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
static
int FrameQueue_Enqueue(c_Queue_t* Queue,__In char* Data,unsigned int DataLength)
{
	if(DataLength>=FrameQueue_RemBufferSize(Queue)) return Queue_OPRST_NoEnoughBuffer;
	char* bufferd;
	Object_cfg(Queue,cfg);
	int _rst=0;
	int remlen=DataLength;
	unsigned int ElementSize=FrameQueue_GetElementSize();
	int WholeFrameFlag=(ElementSize>1)?0:-1;
	
	do
	{
		_rst=FrameQueue_AskEnqueue(Queue,&bufferd);
		if(_rst>remlen)
		{
			_rst=remlen;
		}
		else if(_rst==remlen)
		{
			WholeFrameFlag++;
		}
		if (Data)
		{
			memcpy(bufferd,Data,(unsigned int)_rst);
			Data+=_rst;	
		}
		remlen-=_rst;
		unsigned int eCount=(ElementSize==1)?_rst:FrameQueue_UpdateElementHeader(Queue,_rst);
		FrameQueue_IncWriteIndex(Queue,eCount);
	}while(remlen>0);
	
	if(WholeFrameFlag>0)
	{
		Queue->State.AlarmState.PartFrameFlag=1;
//		return Queue_OPRST_ThisFrameIsPart;
	}
	else
	{
		Queue->State.AlarmState.PartFrameFlag=0;
	}
	return DataLength-remlen;
}

static
int FrameQueue_Clear(c_Queue_t* Queue)
{
	Queue->ReadIndex=0;
	Queue->WriteIndex=0;
	Queue->ElementCount=0;
	Queue->State.Value=0;
	Queue->State.AlarmState.EmptyFlag=1;
	return 0;
}

static
int FrameQueue_FirstFrameLength (c_FrameQueue_t *Queue)
{
    class_internal(Queue, ptThis, c_FrameQueue_t);
    protected_internal(&this->use_as__c_Queue_t, ptBase, c_Queue_t);

	if(this->ElementCount<=0) return 0;
	Object_cfg(this,cfg);
	unsigned int ElementSize=FrameQueue_GetElementSize();
	
	if(ElementSize>1)
	{
		int Length=0;
		typeof(this->ReadIndex) id=this->ReadIndex;
		
		s_ElementsHeader_t* Header=FrameQueue_GetElementHeader(base,id);
		if(id>=this->WriteIndex)
		{
			for(;id<cfg->ElementCount;id++)
			{
				Length+=Header->ByteCount;
				if(Header->ByteCount<ElementSize)
				{
					return Length;
				}
				FrameQueue_ElementsHeaderToNext(Header);
			}
			id=0;
			Header=FrameQueue_GetElementHeader(base,id);
		}
		for(;id<this->WriteIndex;id++)
		{
			Length+=Header->ByteCount;
			if(Header->ByteCount<ElementSize)
			{
				return Length;
			}
			FrameQueue_ElementsHeaderToNext(Header);
		}
		return 0;
	}
	else
	{
		return this->ElementCount;
	}
}

static
u_QueueState_t FrameQueue_GetState(c_Queue_t* Queue)
{
	return Queue->State;
}

const i_FrameQueue_t* const zmFrameQueueManager = &(const i_FrameQueue_t){
    .Create =             &FrameQueue_Create,
    .Dispose =           &FrameQueue_Dispose,
    .Enqueue=			&FrameQueue_Enqueue,
	.Dequeue=			&FrameQueue_Dequeue,
	.AskEnqueue=		&FrameQueue_AskEnqueue,
	.AskDequeue=		&FrameQueue_AskDequeue,
	.ByteCount=			&FrameQueue_ByteCount,
	.RemBufferSize=		&FrameQueue_RemBufferSize,
	.Clear=				&FrameQueue_Clear,
	.FirstFrameLength=		&FrameQueue_FirstFrameLength,
	.GetState=&FrameQueue_GetState
    /* other methods */
};

/* EOF */