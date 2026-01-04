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
#define __SPIMaster_CLASS_INHERIT__
#define __SPISyncMaster_CLASS_IMPLEMENT__

#include "zmSPISyncMaster.h"
#include "HAL_Interface/HAL_Interface_SPI.h"

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

#define SPI_PortIsBusy(__SPINode)		(__SPINode->State & (COM_State_Receivinging | COM_State_Sending))

/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

/*============================ IMPLEMENTATION ================================*/

static
void   SPI_DeInit (c_com_t* const me)
{
	this_from_implenment(me,c_com_t,c_SPIMasterNode_t);
	with(this)
	{
		_->EventHandler=NULL;
	}	
}

static
e_ComOperationResult_t  SPI_Init(c_com_t* const me, s_com_init_t *InitParm,const i_comEventHandler_t* const EventHandler)
{
	this_from_implenment(me,c_com_t,c_SPIMasterNode_t);
	
	with(this)
	{
		_->EventHandler=EventHandler;
		_->InitSetting=InitParm->ComSetting;
		zmGPIOManager->WritePin(_->CSPin,GPIO_Value_HighLevel);
		zmGPIOManager->SetMode(_->CSPin,GPIO_MD_PushPull);
	}
	return COM_OPRST_Success;
}

static
e_ComOperationResult_t  SPI_Open(c_com_t* const me,void* RunParm)
{
	this_from_implenment(me,c_com_t,c_SPIMasterNode_t);
	char Index=zmCOM_GetIndexInType(this->PortIndex);
	UH_SPI_Init(Index, this->InitSetting);

	s_SPIPortOpenRunParm_t* OpenParm=(s_SPIPortOpenRunParm_t*)RunParm;
	if (OpenParm && OpenParm->SPIPortConfig)
	{
		UH_SPI_UpdateSetting(Index,OpenParm->SPIPortConfig);
	}
	const u_SPIFrameFormat_t* SPIFormat=(const u_SPIFrameFormat_t*)&this->InitSetting->TransFormat;
	return COM_OPRST_Success;
}

static
e_ComOperationResult_t  SPI_Close(c_com_t* const me)
{
	this_from_implenment(me,c_com_t,c_SPIMasterNode_t);
	char Index=zmCOM_GetIndexInType(this->PortIndex);
	UH_SPI_DeInit(Index);
	return COM_OPRST_Success;
	
}

static int SPI_ReadWrite(c_SPIMasterNode_t* const me,char* ReadTo,int datalength,e_COM_State_t WorkState)
{
	if(datalength<=0) return COM_OPRST_ParmFail;
	class_internal(me, ptThis, c_SPIMasterNode_t);	
	char Index=zmCOM_GetIndexInType(this->PortIndex);
	if(SPI_PortIsBusy(this))
	{
		return COM_OPRST_PortIsBusy;
	}	
	this->State |= WorkState;
	zmGPIOManager->WritePin(this->CSPin,GPIO_Value_LowLevel);
	int len= UH_SPI_ReadWrite(Index,ReadTo,datalength);
	this->State &= ~WorkState;
	zmGPIOManager->WritePin(this->CSPin,GPIO_Value_HighLevel);
	if(len>0)
	{
		return len;
	}
	else
	{
		return COM_OPRST_NoEnoughMemory;
	}
}

static
int  SPI_Read(c_com_t* const me,char* ReadTo,int datalength)
{
	this_from_implenment(me,c_com_t,c_SPIMasterNode_t);
	return SPI_ReadWrite(this,ReadTo,datalength,COM_State_Receivinging);
}

static
int  SPI_Write(c_com_t* const me,const char* DataFrom,int datalength)
{
	this_from_implenment(me,c_com_t,c_SPIMasterNode_t);
	return SPI_ReadWrite(this,(char*)DataFrom,datalength,COM_State_Sending);
}

static
int  SPI_ReceiveByteCount(c_com_t* const me,bool FirstFrameFlag)
{
	return 0;
}

static
int  SPI_WriteBufferRemByteCount(c_com_t* const me)
{
	return 0;
}

static
e_COM_State_t  SPI_GetState(c_com_t* const me)
{
	char Index=zmCOM_GetIndexInType(me->PortIndex);
	me->State &=~COM_State_BaseMak;
	me->State |= (UH_SPI_IsBusy(Index))?COM_State_Connected:COM_State_PortClosed;
	return me->State;
}

static
char*  SPI_PickReceivedStream(c_com_t* const me,int* datalength)
{
	return NULL;
}

static
char*  SPI_AskWriteBuffer(c_com_t* const me,int* datalength)
{
	return NULL;
}

static
int  SPI_GetSetting(c_com_t* const me,s_com_Setting_t* Parm)
{
	char Index=zmCOM_GetIndexInType(me->PortIndex);
	return UH_SPI_GetSetting(Index,Parm);
}

static 
int  SPI_UpdateSetting(c_com_t* const me,const s_com_Setting_t* const Parm)
{
	char Index=zmCOM_GetIndexInType(me->PortIndex);
	if(UH_SPI_IsBusy(Index)) return COM_OPRST_PortIsBusy;
	int Rst=UH_SPI_UpdateSetting(Index,Parm);
	return Rst;	
}

const i_SPIMaster_t	g_zmSPISyncMasterManager ={
	.Name="SPI同步主节点管理器",
	.Init=SPI_Init,
	.DeInit=SPI_DeInit,
	.Open=SPI_Open,
	.Close=SPI_Close,
	.GetState=SPI_GetState,
	.Read=SPI_Read,
	.Write=SPI_Write,
	.ReceiveByteCount=SPI_ReceiveByteCount,
	.WriteBufferRemByteCount=SPI_WriteBufferRemByteCount,
	.PickReceivedStream=SPI_PickReceivedStream,
	.AskWriteBuffer=SPI_AskWriteBuffer,
	.GetSetting=SPI_GetSetting,
	.UpdateSetting=SPI_UpdateSetting
    /* other methods */
};



/* EOF */