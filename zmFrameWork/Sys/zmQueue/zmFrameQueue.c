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
/*本文件基于PLOOC开发，以上是原版说明*/
/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： zmFrameQueue.c 
开发日期：2025-03-28 
文件功能：实现帧数据的队列式管理

描    述：本对象管理一段连续的缓存区，将这段缓存区分割为n个单元，每个单元称为1个元素，可实现对不定长帧数据的队列式管理。
			元素大小最小为1个字节，帧长度可以大于、小于或等于元素大小，当等于元素大小时，本对象会以后面1个元素为帧尾标志，此时空间利用率最低。
		用户可通过在创建队列对象时指定元素大小和元素个数，在空间利用率和执行效率方面寻找均衡。

依赖接口：

提供接口：zmFrameQueueManager对象
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

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
* 函 数 名： int
* 函数功能： 申请出队
* 参    数： Queue: 队列对象
**			 DataBuffer: [输出]：队首的数据缓存地址
* 返    回：若成功，返回本次可出队的数据长度，该长度为一帧数据的实际长度，或者是ElementSize的整数倍
* 描    述：本函数常用于为发送DMA提供数据源区间，在DMA传输结束后，需要调用Dequeue函数将数据正式出队，这种情况下需要将Dequeue中的Data参数设置为空

* 开 发 者：赵明 
* 日    期： 2024-03-24

* 修改历史：
** 1. 新创建

 *****************************************************************************/
static
int FrameQueue_AskDequeue(c_Queue_t* Queue,__Out const char** DataBuffer)
{
	unsigned int eCount;
	return FrameQueue_AskDeQueue_Inner(Queue,DataBuffer,&eCount);
}

/*****************************************************************************
* 函 数 名： Dequeue
* 函数功能： 出队一组数据
* 参    数： Queue: 队列对象 
**			 Data: 数据去向，若为空，则不复制数据 
**			 DataLength: 要出队的数据最大字节数
* 返    回： 若成功，实际出队的数据字节数，否则返回异常码，异常码见e_QueueOperationResult_t
* 描    述： 当DataLength大于等于队列中第一整帧数据长度时，出队一帧数据，并返回实际出队的字节量；
			 当DataLength小于队列中第一整帧数据长度时，出队的数据量为L,L=DataLength-DataLength % ElementSize,并返回Queue_OPRST_DstBufferOverflow；

* 开 发 者：赵明 
* 日    期： 2024-03-24

* 修改历史：
** 1. 新创建

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
* 函 数 名： AskEnqueue
* 函数功能： 申请入队
* 参    数： Queue: 队列对象 
**			 DataBuffer: [输出]：申请到的缓存地址
* 返    回： 若成功，返回申请到的缓存长度，该长度为ElementSize的整数倍
* 描    述： 本函数常用于为接收DMA提供缓存空间，在DMA传输结束后，需要调用Enqueue函数将数据正式入队，这种情况下需要将Enqueue中的Data参数设置为空

* 开 发 者：赵明 
* 日    期： 2024-03-24

* 修改历史：
** 1. 新创建

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
* 函 数 名： Enqueue
* 函数功能： 入队一个或多个元素
* 参    数： Queue: 队列对象 
**			 Data: [输入]：要入队的数据流,若为空，不复制数据
**			 DataLength: 要入队的数据流长度 
* 返    回： 若成功，返回实际入队的数据长度，否则返回异常码，异常码见e_QueueOperationResult_t
* 描    述： 入队的数据流可小于元素大小，此时占用一个元素的空间；
			 也可大于元素大小，此时占用多个元素空间，且向上取整；
			若缓存区满，会覆盖掉早期数据

* 开 发 者：赵明 
* 日    期： 2024-03-24

* 修改历史：
** 1. 新创建

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