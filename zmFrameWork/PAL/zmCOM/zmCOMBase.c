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
/*本文件基于PLOOC定制，以上是原版说明*/

/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： zmCOMBase.c 
开发日期：2025-03-26 
文件功能：作为数字通信接口对象的基类，实现数字通信接口的通用方法。

描    述：使用帧队列对输入输出数据进行缓存

依赖接口：

提供接口：
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

/*============================ INCLUDES ======================================*/
#define __com_CLASS_IMPLEMENT__

#include "zmCOMBase.h"

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
#define this        (ptThis)

#define MyQueueManager		zmFrameQueueManager

/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

/*============================ IMPLEMENTATION ================================*/


/*============================ protected 方法================================*/

int  COM_Read(const c_com_t* const me,__Out char* ReadTo,int datalength)
{
	if(datalength<=0) return COM_OPRST_ParmFail;
	class_internal(me, ptThis, c_com_t);	
	class_internal(this->ReceiveQueue, Queue, c_Queue_t);	
	
	char PortIndex=this->PortIndex;
	if (MyQueueManager->ByteCount(Queue) <=0)
	{
		return 0;
	}
	else
	{
		int tl=MyQueueManager->Dequeue(Queue,ReadTo,datalength);
//		 if(tl<0)
//		{	
//			return COM_OPRST_GetRxDataFailed;
//		}
		return tl;
	}
}

int  COM_Write(const c_com_t* const me,__In char* Data,int datalength)
{
	class_internal(me, ptThis, c_com_t);		
	class_internal(this->SendQueue, Queue, c_Queue_t);	
	char PortIndex=this->PortIndex;
	char* data;
	int rst=COM_OPRST_Success;
	int newlen=datalength;
	if (MyQueueManager->RemBufferSize(Queue)<newlen)		//检查缓存是否足够
	{
		rst= COM_OPRST_NoEnoughMemory;		
	}
	else
	{
		rst=MyQueueManager->Enqueue(Queue,Data,newlen);
		u_QueueState_t QueueState=MyQueueManager->GetState(Queue);
		if(QueueState.AlarmState.PartFrameFlag) MyQueueManager->Enqueue(Queue,NULL,0);
	}
	return rst;
}

int  COM_ReceiveByteCount(const c_com_t* const me,bool FirstFrameFlag)
{
	class_internal(me, ptThis, c_com_t);		
	class_internal(this->ReceiveQueue, Queue, c_FrameQueue_t);	
	return (FirstFrameFlag)?MyQueueManager->FirstFrameLength(Queue):MyQueueManager->ByteCount(&Queue->use_as__c_Queue_t);
}

 int  COM_WriteBufferRemByteCount(const c_com_t* const me)
{
	class_internal(me, ptThis, c_com_t);		
	class_internal(this->SendQueue, Queue, c_Queue_t);	
	int len=MyQueueManager->RemBufferSize(Queue);
	return len;
}

char*  COM_PickReceivedStream(const c_com_t* const me,int* datalength)
{
	class_internal(me, ptThis, c_com_t);	
	class_internal(this->ReceiveQueue, Queue, c_Queue_t);	
	char* data=NULL;
	int len=MyQueueManager->AskDequeue(Queue,(const char**)&data);
	*datalength=len;
	return data;
}

char*  COM_PickWriteStream(const c_com_t* const me,int* datalength)
{
	class_internal(me, ptThis, c_com_t);	
	class_internal(this->SendQueue, Queue, c_Queue_t);	
	char* data=NULL;
	while(MyQueueManager->ByteCount(Queue)>0)
	{
		int len=MyQueueManager->AskDequeue(Queue,(const char**)&data);
		if(len==0 && data)
		{
			MyQueueManager->Dequeue(Queue,NULL,Queue_DataLength_OneFrame);
			continue;
		}
		else if(len>0)
		{
			*datalength=len;
		}
		break;
	}
	return data;
}

char*  COM_AskWriteBuffer(const c_com_t* const me,int* datalength)
{
	class_internal(me, ptThis, c_com_t);	
	class_internal(this->SendQueue, Queue, c_Queue_t);	
	char* data=NULL;
	int len=MyQueueManager->AskEnqueue(Queue,&data);
	*datalength=len;
	return data;
}

void COM_ClearBuffer(const c_com_t* const me)
{
	//清空缓存区
	class_internal(me, ptThis, c_com_t);	
	class_internal(this->ReceiveQueue, RQueue, c_Queue_t);	
	if(RQueue) MyQueueManager->Clear(RQueue);
	class_internal(this->ReceiveQueue, TQueue, c_Queue_t);	
	if(TQueue) MyQueueManager->Clear(TQueue);
}

/**********************************************驱动的回调函数区*********************************************************/
#define COM_OnDataSendedEvent(__FrameFinishFlag)					COM_OnEvent(DataSended,this,__FrameFinishFlag)

int UHC_COM_PacketSended(const c_com_t* const me,__InOut const char** data,__InOut int* datalength)
{
	class_internal(me, ptThis, c_com_t);	
	class_internal(this->SendQueue, Queue, c_Queue_t);	
	if(Queue)
	{
		int SendedLen=*datalength;
		int len=MyQueueManager->Dequeue(Queue,NULL,SendedLen);
		if(len<0)
		{
			COM_PrintfError("串口%d包发完了，出队时出现了异常(%d)\n",this->PortIndex,len);
			return len;
		}
		u_QueueState_t QueueState=MyQueueManager->GetState(Queue);
		
		int rst=0;
		if(QueueState.AlarmState.PartFrameFlag) //如果本帧尚未发完
		{
			len=MyQueueManager->AskDequeue(Queue,data);
			*datalength=len;
			rst=len;
			if(len==0) //发完了
			{
				MyQueueManager->Dequeue(Queue,NULL,COM_DataLength_OneFrame);
				QueueState.AlarmState.PartFrameFlag=0;
			}
			else if(len<0)
			{
				COM_PrintfError("通信口%d包发完了，前半帧出队成功，后半帧出队时出现了异常(%d)\n",this->PortIndex,len);
			}
		}
		COM_OnDataSendedEvent(!QueueState.AlarmState.PartFrameFlag);
		return rst;
	}
	else
	{
		COM_OnDataSendedEvent(1);
		return 0;
	}
}

#define COM_OnDataReceivedEvent(__FrameFinishFlag)				COM_OnEvent(DataReceived,this,__FrameFinishFlag)

void UHC_COM_DataReceived(const c_com_t* const me,unsigned int DataLength,bool BufferFulledFlag)
{
	class_internal(me, ptThis, c_com_t);	
	class_internal(this->ReceiveQueue, Queue, c_Queue_t);	
	if(Queue)
	{
		int len=MyQueueManager->Enqueue(Queue,NULL,DataLength);
		u_QueueState_t QueueState=MyQueueManager->GetState(Queue);	
		if(QueueState.AlarmState.PartFrameFlag && BufferFulledFlag==false)
		{
			len=MyQueueManager->Enqueue(Queue,NULL,0);
		}
		COM_OnDataReceivedEvent(QueueState.AlarmState.PartFrameFlag);
	}
	else
	{
		COM_OnDataReceivedEvent(true);
	}
}

void UHC_COM_Connected(const c_com_t* const me)
{
	class_internal(me, ptThis, c_com_t);	
	COM_OnEvent(Connected,this->PortIndex);
}

void UHC_COM_DisConnected(const c_com_t* const me)
{
	class_internal(me, ptThis, c_com_t);	
	COM_OnEvent(DisConnected,this->PortIndex);
	COM_ClearBuffer(me);
}



const i_com_t* const com = &(const i_com_t){

    
    /* other methods */
};


/* EOF */