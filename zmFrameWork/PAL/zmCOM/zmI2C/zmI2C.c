/*本文件基于PLOOC定制，以下是原版说明*/
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


/*============================ INCLUDES ======================================*/

#define __com_CLASS_INHERIT__
#define __List_CLASS_IMPLEMENT__
#define __I2C_CLASS_IMPLEMENT__

#include "zmI2C.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "HAL_Interface/HAL_Interface_I2C.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)    assert(__VA_ARGS__)
#endif

#undef this
#define this        ptThis

#undef base
#define base        ptBase
#define MyQueueManager		zmFrameQueueManager

/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
static c_List_t _g_PortList={.Head=NULL};
#define g_PortList	(&_g_PortList)

/*============================ PROTOTYPES ====================================*/
inline static bool I2CNode_Contains(const c_I2CNode_t* I2CNode)
{
	
	class_internal(I2CNode, ptThis, c_I2CNode_t);		
	protected_internal(&this->use_as__c_ListItem_t,ListItem,c_ListItem_t);
	
	return zmListManager->Contains(g_PortList,ListItem);
}

static bool I2CNode_FindCHeckFun(c_ListItem_t* Item,const void* FindObject)
{
	this_from_implenment(Item,c_ListItem_t,c_I2CNode_t);
	
	typeof(this->PortIndex) value=*(typeof(this->PortIndex)*)FindObject;
	return this->PortIndex==value;
}

static c_I2CNode_t* I2CNode_Find(typeof(((const c_com_t*)0)->PortIndex) PortIndex)
{
	c_ListItem_t* Item=zmListManager->Find(g_PortList,&PortIndex,I2CNode_FindCHeckFun);
	if(Item==NULL) return NULL;
	this_from_implenment(Item,c_ListItem_t,c_I2CNode_t);
	return this;
}

static void I2CNode_Add(const c_I2CNode_t* I2CNode)
{
	class_internal(I2CNode, ptThis, c_I2CNode_t);		
	protected_internal(&this->use_as__c_ListItem_t,ListItem,c_ListItem_t);
	return zmListManager->Add(g_PortList,ListItem);
}

static void I2CNode_Remove(const c_com_t* I2CNode)
{
	class_internal(I2CNode, ptThis, c_I2CNode_t);		
	protected_internal(&this->use_as__c_ListItem_t,ListItem,c_ListItem_t);
	return zmListManager->Remove(g_PortList,ListItem);
}

static void I2CNode_SendData(c_I2CNode_t* I2CNode,const char* data,int sendlen)
{
	I2CNode->SendingLength=sendlen;
	char Index=zmCOM_GetIndexInType(I2CNode->PortIndex);
	UH_I2C_TransData(Index,I2CNode->InitSetting, data,sendlen);
}

/*============================ IMPLEMENTATION ================================*/
static
void   I2C_DeInit (c_com_t* const me)
{
	I2CNode_Remove(me);
	class_internal(me, ptThis, c_I2CNode_t);		
	protected_internal(&this->use_as__c_com_t,ptBase,c_com_t);
	with(this)
	{
		_->EventHandler=NULL;
		if(_->ReceiveQueue) MyQueueManager->Dispose(&_->ReceiveQueue->use_as__c_Queue_t);
		if(_->SendQueue) MyQueueManager->Dispose(&_->SendQueue->use_as__c_Queue_t);
	}	
}

static
e_ComOperationResult_t  I2C_Init(c_com_t* const me, s_com_init_t *InitParm,const i_comEventHandler_t* const EventHandler)
{
	const s_Queue_cfg_t _DefaultQueueConfig=Append_QueueConfig(I2C_FrameSizeLevel_Default,I2C_QueueFrameCount_Default);

	class_internal(me, ptThis, c_I2CNode_t);		
	protected_internal(&this->use_as__c_com_t,ptBase,c_com_t);
	if(I2CNode_Contains(this)) I2C_DeInit(me);
	
	if(InitParm==NULL) InitParm=&(typeof(*InitParm)){.ReceiveQueueConfig=&_DefaultQueueConfig,.SendQueueConfig=&_DefaultQueueConfig};

	with(this)
	{
		_->EventHandler=EventHandler;
		if(_->ReceiveQueue==NULL) _->ReceiveQueue=(typeof(_->ReceiveQueue))MyQueueManager->Create(InitParm->ReceiveQueueConfig);
		if(_->ReceiveQueue==NULL) return COM_OPRST_MallocFail;
		if(_->SendQueue==NULL) _->SendQueue=(typeof(_->ReceiveQueue))MyQueueManager->Create(InitParm->SendQueueConfig);
		if(_->SendQueue==NULL) return COM_OPRST_MallocFail;
	}
	I2CNode_Add(this);
	return COM_OPRST_Success;
}


static void I2C_BeginRx(c_I2CNode_t* I2CNode,char* To,int length)
{
	class_internal(I2CNode, ptThis, c_I2CNode_t);		
	c_Queue_t* Queue=&this->ReceiveQueue->use_as__c_Queue_t;
	char* data;
	int Maxlength;
	do 
	{
		Maxlength=MyQueueManager->AskEnqueue(Queue,&data);
		switch(Maxlength)
		{
		case Queue_OPRST_Fulled:
			MyQueueManager->Dequeue(Queue,NULL,0xffff);
			break;

		default:
			break;
		}
	} while (Maxlength<=0);
	char Index=zmCOM_GetIndexInType(this->PortIndex);
	*data=(this->InitSetting->I2CNodeAddr<<1)+1;
	UH_I2C_TransData(Index,this->InitSetting, data,MIN(Maxlength,length));
}

static
e_ComOperationResult_t  I2C_Open(const c_com_t* const me,void* RunParm)
{
	class_internal(me, ptThis, c_I2CNode_t);		
	protected_internal(&this->use_as__c_com_t,ptBase,c_com_t);
	char Index=zmCOM_GetIndexInType(this->PortIndex);
	UH_I2C_Init(Index, this->InitSetting);
	this->State=COM_State_Connected;
	return COM_OPRST_Success;

}

static
e_ComOperationResult_t  I2C_Close(const c_com_t* const me)
{
	class_internal(me, ptThis, c_I2CNode_t);		
	class_internal(this->ReceiveQueue, Queue, c_Queue_t);	
	this->State=COM_State_DisConnected;
	MyQueueManager->Clear(Queue);
	class_internal(this->SendQueue, Queue1, c_Queue_t);	
	MyQueueManager->Clear(Queue1);
	char Index=zmCOM_GetIndexInType(this->PortIndex);
	UH_I2C_DeInit(Index);
	return COM_OPRST_Success;
	
}

static
int  I2C_ReceiveByteCount(const c_com_t* const me,bool FirstFrameFlag)
{
	class_internal(me, ptThis, c_I2CNode_t);	
	if(this->ReceiveQueue==NULL) return 0;
	class_internal(this->ReceiveQueue, Queue, c_Queue_t);	
	return COM_ReceiveByteCount(me,FirstFrameFlag);
}

static
int  I2C_Read(const c_com_t* const me,char* ReadTo,int datalength)
{
	class_internal(me, ptThis, c_I2CNode_t);	
	if(I2C_ReceiveByteCount(me,false)<=0)
	{
		char Index=zmCOM_GetIndexInType(this->PortIndex);
		if(UH_I2C_IsBusy(Index))
		{
			//有待增加发送队列，用于处理并发请求
			this->AskTransFlag|=I2C_ATFlag_Read;
			return COM_OPRST_PortIsBusy;
		}
		else
		{
			I2C_BeginRx(this,ReadTo,datalength);
			return COM_OPRST_BeginExecuted;
		}
	}
	else
	{
		return COM_Read(me,ReadTo,datalength);
	}
#if 0
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
#endif
}

static
int  I2C_Write(const c_com_t* const me,const char* DataFrom,int datalength)
{
	if(datalength<=0) return COM_OPRST_ParmFail;
	class_internal(me, ptThis, c_I2CNode_t);		
	class_internal(this->SendQueue, Queue, c_Queue_t);	
	char Index=zmCOM_GetIndexInType(this->PortIndex);
	int rst=COM_Write(me,DataFrom,datalength);

	//如果当前端口没有处于发送状态，则启动发送
	if(UH_I2C_IsBusy(Index))
	{
		this->AskTransFlag|=I2C_ATFlag_Write;
		return COM_OPRST_PortIsBusy;
	}
	else
	{
		const char* data=COM_PickWriteStream(me,&datalength);
		if (datalength>0)
		{
			I2CNode_SendData(this,data,datalength);
		}
		else
		{
			I2C_PrintfError("发送数据已经正常入队，申请出队旧数据时失败,异常码=%d\n",datalength);
			if(rst>=0) return COM_OPRST_LoadSendDataFailed;		//返回异常
		}
	}
	return rst;
}

static
int  I2C_WriteBufferRemByteCount(const c_com_t* const me)
{
	return COM_WriteBufferRemByteCount(me);
}

static
e_COM_State_t  I2C_GetState(const c_com_t* const me)
{
	class_internal(me, ptThis, c_I2CNode_t);		
	char Index=zmCOM_GetIndexInType(me->PortIndex);
	return (UH_I2C_IsBusy(Index))?COM_State_Busy:this->State;
}

static
char*  I2C_PickReceivedStream(const c_com_t* const me,int* datalength)
{
	return COM_PickReceivedStream(me,datalength);
}

static
char*  I2C_AskWriteBuffer(const c_com_t* const me,int* datalength)
{
	return COM_AskWriteBuffer(me,datalength);
}

static
int  I2C_GetSetting(const c_com_t* const me,s_com_Setting_t* Parm)
{
	char Index=zmCOM_GetIndexInType(me->PortIndex);
	return UH_I2C_GetSetting(Index,Parm);
}

static 
int  I2C_UpdateSetting(const c_com_t* const me,const s_com_Setting_t* const Parm)
{
	char Index=zmCOM_GetIndexInType(me->PortIndex);
	if(UH_I2C_IsBusy(Index)) return COM_OPRST_PortIsBusy;
	int Rst=UH_I2C_UpdateSetting(Index,Parm);
	if(Rst==0)
	{
		UHC_I2C_SettingCHanged(Index,Parm);
	}
	return Rst;
	
}

inline static void I2C_ClearBuffer(const c_I2CNode_t* const me)
{
	return COM_ClearBuffer(&me->use_as__c_com_t);
}


/**********************************************驱动的回调函数区*********************************************************/
e_I2CSendCallBackResult_t UHC_I2C_PacketSended(int PortIndex,char** data,int* datalength)
{
	char _TotalPortIndex=zmCOM_CreatePortID(PortIndex,zmCOM_Type_I2C);
	c_I2CNode_t* me=I2CNode_Find(_TotalPortIndex);
	if(me==NULL) return I2C_SENDCBRT_SendOver;
	*datalength=me->SendingLength;
	int len=UHC_COM_PacketSended(&me->use_as__c_com_t,(const char**)data,datalength);
	me->SendingLength=*datalength;
	return (len>0)?I2C_SENDCBRT_NeedSending:I2C_SENDCBRT_SendOver;
}

void UHC_I2C_DataSended(int PortIndex)
{
	char _TotalPortIndex=zmCOM_CreatePortID(PortIndex,zmCOM_Type_I2C);
	c_I2CNode_t* me=I2CNode_Find(_TotalPortIndex);
	if(me==NULL) return;
	int length=0;
	char* data=COM_PickWriteStream(&me->use_as__c_com_t,&length);;
	if(length>0)
	{
		I2CNode_SendData(me,data,length);
	}
}

void UHC_I2C_DataReceived(int PortIndex,unsigned int DataLength)
{
	char _TotalPortIndex=zmCOM_CreatePortID(PortIndex,zmCOM_Type_I2C);
	c_I2CNode_t* me=I2CNode_Find(_TotalPortIndex);
	if(me==NULL) return;
	UHC_COM_DataReceived(&me->use_as__c_com_t,DataLength,false);
}

void UHC_I2C_Connected(int PortIndex)
{
	char _TotalPortIndex=zmCOM_CreatePortID(PortIndex,zmCOM_Type_I2C);
	const c_I2CNode_t* me=I2CNode_Find(_TotalPortIndex);
	if(me==NULL) return;
	UHC_COM_Connected(&me->use_as__c_com_t);
}

void UHC_I2C_DisConnected(int PortIndex)
{
	char _TotalPortIndex=zmCOM_CreatePortID(PortIndex,zmCOM_Type_I2C);
	const c_I2CNode_t* me=I2CNode_Find(_TotalPortIndex);
	if(me==NULL) return;
	UHC_COM_DisConnected(&me->use_as__c_com_t);
}


void UHC_I2C_SettingCHanged(int PortIndex,const void* const Setting)
{
	char _TotalPortIndex=zmCOM_CreatePortID(PortIndex,zmCOM_Type_I2C);
	const c_I2CNode_t* me=I2CNode_Find(_TotalPortIndex);
	if(me==NULL) return;
	
	class_internal(me, ptThis, c_I2CNode_t);	
	const i_comEventHandler_t* callback=this->EventHandler;	
	if (callback && callback->SettingCHanged)	
	{	
		callback->SettingCHanged(&me->use_as__c_com_t,Setting);	
	}	
}


void UHC_I2C_Error(int PortIndex,e_I2CErrorType_t ErrorType)
{
	char _TotalPortIndex=zmCOM_CreatePortID(PortIndex,zmCOM_Type_I2C);
	const c_I2CNode_t* me=I2CNode_Find(_TotalPortIndex);
	if(me==NULL) return;
	
	class_internal(me, ptThis, c_I2CNode_t);	
	const i_comEventHandler_t* callback=this->EventHandler;	
	if (callback && callback->DataError)	
	{	
		callback->DataError(&me->use_as__c_com_t,ErrorType);	
	}	
}

const i_I2C_t* const zmI2CManager = &(const i_I2C_t){.Name="I2C管理器",
													.Init= I2C_Init,
													.DeInit=I2C_DeInit,
													.Open=I2C_Open,
													.Close=I2C_Close,
													.GetState=I2C_GetState,
													.Read=I2C_Read,
													.Write=I2C_Write,
													.ReceiveByteCount=I2C_ReceiveByteCount,
													.WriteBufferRemByteCount=I2C_WriteBufferRemByteCount,
													.PickReceivedStream=I2C_PickReceivedStream,
													.AskWriteBuffer=I2C_AskWriteBuffer,
													.GetSetting=I2C_GetSetting,
													.UpdateSetting=I2C_UpdateSetting

};
/* EOF */