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
文 件 名： zmSerialPort.c 
开发日期：2025-03-26 
文件功能：实现串口管理器类对象

描    述：

依赖接口：HAL_Interface_Uart.h 文件中的接口

提供接口：
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

/*============================ INCLUDES ======================================*/
#define __com_CLASS_INHERIT__
#define __List_CLASS_IMPLEMENT__
//#define __ListItem_CLASS_IMPLEMENT__
#define __SerialPort_CLASS_IMPLEMENT__

#include "./zmSerialPort.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "HAL_Interface/HAL_Interface_Uart.h"

/*============================ MACROS ========================================*/


/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)    assert(__VA_ARGS__)
#endif

#undef this
#define this        ptThis

#undef base
#define base        ptBase

//#define this_from_implenment(__obj, __type,__thistype)			class_internal((unsigned int)__obj-offsetof(__thistype,use_as__##__type),ptThis,__thistype)
#define MyQueueManager		zmFrameQueueManager

/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
static c_List_t _g_PortList={.Head=NULL};
#define g_PortList	(&_g_PortList)

/*============================ PROTOTYPES ====================================*/
inline static bool SerialPort_Contains(const c_SerialPort_t* SerialPort)
{
	
	class_internal(SerialPort, ptThis, c_SerialPort_t);		
	protected_internal(&this->use_as__c_ListItem_t,ListItem,c_ListItem_t);
	
	return zmListManager->Contains(g_PortList,ListItem);
}

static bool SerialPort_FindCHeckFun(c_ListItem_t* Item,const void* FindObject)
{
	this_from_implenment(Item,c_ListItem_t,c_SerialPort_t);
	
	typeof(this->PortIndex) value=*(typeof(this->PortIndex)*)FindObject;
	return this->PortIndex==value;
}

static c_SerialPort_t* SerialPort_Find(typeof(((const c_com_t*)0)->PortIndex) PortIndex)
{
	c_ListItem_t* Item=zmListManager->Find(g_PortList,&PortIndex,SerialPort_FindCHeckFun);
	if(Item==NULL) return NULL;
	this_from_implenment(Item,c_ListItem_t,c_SerialPort_t);
	return this;
}

static void SerialPort_Add(const c_SerialPort_t* SerialPort)
{
	class_internal(SerialPort, ptThis, c_SerialPort_t);		
	protected_internal(&this->use_as__c_ListItem_t,ListItem,c_ListItem_t);
	return zmListManager->Add(g_PortList,ListItem);
}

static void SerialPort_Remove(const c_com_t* SerialPort)
{
	class_internal(SerialPort, ptThis, c_SerialPort_t);		
	protected_internal(&this->use_as__c_ListItem_t,ListItem,c_ListItem_t);
	return zmListManager->Remove(g_PortList,ListItem);
}

static void SerialPort_SendData(c_SerialPort_t* SerialPort,const char* data,int sendlen)
{
	SerialPort->SendingLength=sendlen;
	char Index=zmCOM_GetIndexInType(SerialPort->PortIndex);
	UH_UART_SendData(Index,data,sendlen);
}
/*============================ IMPLEMENTATION ================================*/
static
void   SerialPort_DeInit (c_com_t* const me)
{
	SerialPort_Remove(me);
	class_internal(me, ptThis, c_SerialPort_t);		
	protected_internal(&this->use_as__c_com_t,ptBase,c_com_t);
	with(this)
	{
		_->EventHandler=NULL;
		if(_->ReceiveQueue) MyQueueManager->Dispose(&_->ReceiveQueue->use_as__c_Queue_t);
		if(_->SendQueue) MyQueueManager->Dispose(&_->SendQueue->use_as__c_Queue_t);
	}	
}

static
e_ComOperationResult_t  SerialPort_Init(c_com_t* const me, s_com_init_t *InitParm,const i_comEventHandler_t* const EventHandler)
{
	const s_Queue_cfg_t _DefaultQueueConfig=Append_QueueConfig(SerialPort_FrameSizeLevel_Default,SerialPort_QueueFrameCount_Default);

	class_internal(me, ptThis, c_SerialPort_t);		
	protected_internal(&this->use_as__c_com_t,ptBase,c_com_t);
	if(SerialPort_Contains(this)) SerialPort_DeInit(me);
	
	if(InitParm==NULL) InitParm=&(typeof(*InitParm)){.ReceiveQueueConfig=&_DefaultQueueConfig,.SendQueueConfig=&_DefaultQueueConfig};

	with(this)
	{
		_->EventHandler=EventHandler;
		if(_->ReceiveQueue==NULL) _->ReceiveQueue=(typeof(_->ReceiveQueue))MyQueueManager->Create(InitParm->ReceiveQueueConfig);
		if(_->ReceiveQueue==NULL) return COM_OPRST_MallocFail;
		if(_->SendQueue==NULL) _->SendQueue=(typeof(_->ReceiveQueue))MyQueueManager->Create(InitParm->SendQueueConfig);
		if(_->SendQueue==NULL) return COM_OPRST_MallocFail;
	}
	SerialPort_Add(this);
	return COM_OPRST_Success;
}


static void UART_BeginRx(c_SerialPort_t* SerialPort)
{
	c_Queue_t* Queue=&SerialPort->ReceiveQueue->use_as__c_Queue_t;
	int length;
	char* data;
	do 
	{
		length=MyQueueManager->AskEnqueue(Queue,&data);
		switch(length)
		{
		case Queue_OPRST_Fulled:
			MyQueueManager->Dequeue(Queue,NULL,0xffff);
			break;

		default:
			break;
		}
	} while (length<=0);
	
	char Index=zmCOM_GetIndexInType(SerialPort->PortIndex);
	UH_UART_StartReceived(Index,data,length);
}

static
e_ComOperationResult_t  SerialPort_Open(const c_com_t* const me,void* RunParm)
{
	class_internal(me, ptThis, c_SerialPort_t);		
	protected_internal(&this->use_as__c_com_t,ptBase,c_com_t);
	char Index=zmCOM_GetIndexInType(this->PortIndex);
	UH_UART_Init(Index, this->InitSetting);

	s_UartOpenRunParm_t* OpenParm=(s_UartOpenRunParm_t*)RunParm;
	if (OpenParm && OpenParm->UartConfig)
	{
		UH_UART_UpdateSetting(Index,OpenParm->UartConfig);
	}
	if(UH_UART_IsOpen(Index))
	{
		UART_BeginRx(this);
		return COM_OPRST_Success;
	}
	else
	{
		return COM_OPRST_PortNotInLine;
	}
	
}

static
e_ComOperationResult_t  SerialPort_Close(const c_com_t* const me)
{
	class_internal(me, ptThis, c_SerialPort_t);		
	class_internal(this->ReceiveQueue, Queue, c_Queue_t);	
	MyQueueManager->Clear(Queue);
	class_internal(this->SendQueue, Queue1, c_Queue_t);	
	MyQueueManager->Clear(Queue1);
	char Index=zmCOM_GetIndexInType(this->PortIndex);
	UH_UART_DeInit(Index);
	return COM_OPRST_Success;
	
}

static
int  SerialPort_Read(const c_com_t* const me,char* ReadTo,int datalength)
{
	if(datalength<=0) return COM_OPRST_ParmFail;
	return COM_Read(me,ReadTo,datalength);
#if 0
	class_internal(me, ptThis, c_SerialPort_t);	
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
int  SerialPort_Write(const c_com_t* const me,const char* DataFrom,int datalength)
{
	if(datalength<=0) return COM_OPRST_ParmFail;
	class_internal(me, ptThis, c_SerialPort_t);		
	class_internal(this->SendQueue, Queue, c_Queue_t);	
	char Index=zmCOM_GetIndexInType(this->PortIndex);
	int rst=COM_Write(me,DataFrom,datalength);

	//如果当前串口没有处于发送状态，则启动发送
	if (UH_UART_IsSending(Index)==false)
	{
		const char* data=COM_PickWriteStream(me,&datalength);
		if (datalength>0)
		{
			SerialPort_SendData(this,data,datalength);
		}
		else
		{
			SerialPort_PrintfError("发送数据已经正常入队，申请出队旧数据时失败,异常码=%d\n",datalength);
			if(rst>=0) return COM_OPRST_LoadSendDataFailed;		//返回异常
		}
	}
	return rst;
}

static
int  SerialPort_ReceiveByteCount(const c_com_t* const me,bool FirstFrameFlag)
{
	class_internal(me, ptThis, c_SerialPort_t);		
	class_internal(this->ReceiveQueue, Queue, c_Queue_t);	
	if(this->InitSetting->ReceiveMode==SerialPort_RWMode_Circle)
	{
		zmQueueWriterManager->UpdateWriteIndex(Queue,UH_UART_GetRxOffsetAddr(this->PortIndex));
	}
	return COM_ReceiveByteCount(me,FirstFrameFlag);
}

static
int  SerialPort_WriteBufferRemByteCount(const c_com_t* const me)
{
	return COM_WriteBufferRemByteCount(me);
}

static
e_COM_State_t  SerialPort_GetState(const c_com_t* const me)
{
	char Index=zmCOM_GetIndexInType(me->PortIndex);
	return (UH_UART_IsOpen(Index))?COM_State_Connected:COM_State_PortClosed;
}

static
char*  SerialPort_PickReceivedStream(const c_com_t* const me,int* datalength)
{
	return COM_PickReceivedStream(me,datalength);
}

static
char*  SerialPort_AskWriteBuffer(const c_com_t* const me,int* datalength)
{
	return COM_AskWriteBuffer(me,datalength);
}

static
int  SerialPort_GetSetting(const c_com_t* const me,s_com_Setting_t* Parm)
{
	char Index=zmCOM_GetIndexInType(me->PortIndex);
	return UH_UART_GetSetting(Index,Parm);
}

static 
int  SerialPort_UpdateSetting(const c_com_t* const me,const s_com_Setting_t* const Parm)
{
	char Index=zmCOM_GetIndexInType(me->PortIndex);
	int Rst=UH_UART_UpdateSetting(Index,Parm);
	if(!Uart_RWSettingIsError(Rst))
	{
		UHC_UART_SettingCHanged(Index,Parm);
	}
	return Rst;
	
}

inline static void Uart_ClearBuffer(const c_SerialPort_t* const me)
{
	return COM_ClearBuffer(&me->use_as__c_com_t);
}


/**********************************************驱动的回调函数区*********************************************************/
e_UartSendCallBackResult_t UHC_UART_PacketSended(int UartIndex,char** data,int* datalength)
{
	char PortIndex=zmCOM_CreatePortID(UartIndex);
	c_SerialPort_t* me=SerialPort_Find(PortIndex);
	if(me==NULL) return UART_SENDCBRT_SendOver;
	*datalength=me->SendingLength;
	int len=UHC_COM_PacketSended(&me->use_as__c_com_t,(const char**)data,datalength);
	me->SendingLength=*datalength;
	return (len>0)?UART_SENDCBRT_NeedSending:UART_SENDCBRT_SendOver;
}

void UHC_UART_DataSended(int UartIndex)
{
	char PortIndex=zmCOM_CreatePortID(UartIndex);
	c_SerialPort_t* me=SerialPort_Find(PortIndex);
	if(me==NULL) return;
	int length=0;
	char* data=COM_PickWriteStream(&me->use_as__c_com_t,&length);;
	if(length>0)
	{
		SerialPort_SendData(me,data,length);
	}
}

void UHC_UART_DataReceived(int UartIndex,unsigned int DataLength,bool BufferFulledFlag)
{
	char PortIndex=zmCOM_CreatePortID(UartIndex);
	c_SerialPort_t* me=SerialPort_Find(PortIndex);
	if(me==NULL) return;
	UHC_COM_DataReceived(&me->use_as__c_com_t,DataLength,BufferFulledFlag);
	UART_BeginRx(me);
}

void UHC_UART_Connected(int UartIndex)
{
	char PortIndex=zmCOM_CreatePortID(UartIndex);
	const c_SerialPort_t* me=SerialPort_Find(PortIndex);
	if(me==NULL) return;
	UHC_COM_Connected(&me->use_as__c_com_t);
}

void UHC_UART_DisConnected(int UartIndex)
{
	char PortIndex=zmCOM_CreatePortID(UartIndex);
	const c_SerialPort_t* me=SerialPort_Find(PortIndex);
	if(me==NULL) return;
	UHC_COM_DisConnected(&me->use_as__c_com_t);
}


void UHC_UART_SettingCHanged(int UartIndex,const void* const Setting)
{
	char PortIndex=zmCOM_CreatePortID(UartIndex);
	const c_SerialPort_t* me=SerialPort_Find(PortIndex);
	if(me==NULL) return;
	
	class_internal(me, ptThis, c_SerialPort_t);	
	const i_comEventHandler_t* callback=this->EventHandler;	
	if (callback && callback->SettingCHanged)	
	{	
		callback->SettingCHanged(&me->use_as__c_com_t,Setting);	
	}	
}


void UHC_UART_Error(int UartIndex,e_SerialPortErrorType_t ErrorType)
{
	char PortIndex=zmCOM_CreatePortID(UartIndex);
	const c_SerialPort_t* me=SerialPort_Find(PortIndex);
	if(me==NULL) return;
	
	class_internal(me, ptThis, c_SerialPort_t);	
	class_internal(this->ReceiveQueue, Queue, c_Queue_t);	
	UART_BeginRx(this);
	const i_comEventHandler_t* callback=this->EventHandler;	
	if (callback && callback->DataError)	
	{	
		callback->DataError(&me->use_as__c_com_t,ErrorType);	
	}	
}

/*============================ GLOBAL VARIABLES ==============================*/
const i_SerialPort_t* const zmSerialPortManager = &(const i_SerialPort_t){.Name="串口管理器",
																		.Init= SerialPort_Init,
																		.DeInit=SerialPort_DeInit,
																		.Open=SerialPort_Open,
																		.Close=SerialPort_Close,
																		.GetState=SerialPort_GetState,
																		.Read=SerialPort_Read,
																		.Write=SerialPort_Write,
																		.ReceiveByteCount=SerialPort_ReceiveByteCount,
																		.WriteBufferRemByteCount=SerialPort_WriteBufferRemByteCount,
																		.PickReceivedStream=SerialPort_PickReceivedStream,
																		.AskWriteBuffer=SerialPort_AskWriteBuffer,
																		.GetSetting=SerialPort_GetSetting,
																		.UpdateSetting=SerialPort_UpdateSetting
    /* other methods */
};

/* EOF */