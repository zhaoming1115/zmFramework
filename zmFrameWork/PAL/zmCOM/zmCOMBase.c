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
�� �� ���� zmCOMBase.c 
�������ڣ�2025-03-26 
�ļ����ܣ���Ϊ����ͨ�Žӿڶ���Ļ��࣬ʵ������ͨ�Žӿڵ�ͨ�÷�����

��    ����ʹ��֡���ж�����������ݽ��л���

�����ӿڣ�

�ṩ�ӿڣ�
	

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

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


/*============================ protected ����================================*/

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
	if (MyQueueManager->RemBufferSize(Queue)<newlen)		//��黺���Ƿ��㹻
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
	//��ջ�����
	class_internal(me, ptThis, c_com_t);	
	class_internal(this->ReceiveQueue, RQueue, c_Queue_t);	
	if(RQueue) MyQueueManager->Clear(RQueue);
	class_internal(this->ReceiveQueue, TQueue, c_Queue_t);	
	if(TQueue) MyQueueManager->Clear(TQueue);
}

/**********************************************�����Ļص�������*********************************************************/
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
			COM_PrintfError("����%d�������ˣ�����ʱ�������쳣(%d)\n",this->PortIndex,len);
			return len;
		}
		u_QueueState_t QueueState=MyQueueManager->GetState(Queue);
		
		int rst=0;
		if(QueueState.AlarmState.PartFrameFlag) //�����֡��δ����
		{
			len=MyQueueManager->AskDequeue(Queue,data);
			*datalength=len;
			rst=len;
			if(len==0) //������
			{
				MyQueueManager->Dequeue(Queue,NULL,COM_DataLength_OneFrame);
				QueueState.AlarmState.PartFrameFlag=0;
			}
			else if(len<0)
			{
				COM_PrintfError("ͨ�ſ�%d�������ˣ�ǰ��֡���ӳɹ������֡����ʱ�������쳣(%d)\n",this->PortIndex,len);
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