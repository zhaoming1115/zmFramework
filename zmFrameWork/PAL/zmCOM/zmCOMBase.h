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
文 件 名： zmCOMBase.h 
开发日期：2025-03-26 
文件功能：为数字通信类端口提供一套统一的接口。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/



#ifndef __ZMCOMBASE_H__
#define __ZMCOMBASE_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../PAL_DataType.h"
#include "../PAL_Config.h"
#include "Sys/Sys.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum
{
	COM_OPRST_Success=0,
	COM_OPRST_FailBase=-16-16,
	COM_OPRST_BeginExecuted,	
	COM_OPRST_MallocFail,		//申请内存失败
	COM_OPRST_ParmFail,		//传入的参数异常
	COM_OPRST_MemoryIsUsed,		//内存占用
	COM_OPRST_PortIsBusy,		//端口忙
	COM_OPRST_ConfigFail,		//配置失败
	COM_OPRST_NoEnoughMemory,		//没有足够的内存
	COM_OPRST_LoadSendDataFailed,		//加载发送数据失败
	COM_OPRST_GetRxDataFailed,		//读取接收到的数据失败
	COM_OPRST_DataIsReceiving,	//数据正在接收
	COM_OPRST_PortNotInLine,	//端口不在线，此项多用于USB等热插拔的接口
	COM_OPRST_NoSurport,	//不支持的操作
	
}e_ComOperationResult_t;	//串行口操作结果类型

#define COM_Assert_Param(__EXPR)			if(__EXPR) return COM_OPRST_ParmFail

typedef enum
{
	COM_State_PortClosed=-1,
	COM_State_WaitConnect,
	COM_State_Connecting,
	COM_State_Connected,
	COM_State_DisConnecting,
	COM_State_DisConnected,
}e_COM_State_t;


typedef struct com_init_t 
{
    //! put your configuration members here
	const s_Queue_cfg_t* ReceiveQueueConfig;
	const s_Queue_cfg_t* SendQueueConfig;
} s_com_init_t;

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__com_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__com_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"

#define COM_DataLength_OneFrame			Queue_DataLength_OneFrame			//数据长度为1帧



//! \name class com_t
//! @{
declare_interface(i_com_t)
declare_interface(i_comEventHandler_t)

declare_class(c_com_t)
def_class(c_com_t,

    public_member(
        //!< place your public members here
		const i_com_t* const Manager;
		const s_com_Setting_t* InitSetting;
		const comid_t PortIndex;
		
   )

    private_member(
        //!< place your private members here
    )
    
    protected_member(
        //!< place your private members here
		e_COM_State_t State;
		const i_comEventHandler_t* EventHandler;
		c_FrameQueue_t* ReceiveQueue;
		c_FrameQueue_t* SendQueue;
    )
)
end_def_class(c_com_t) /* do not remove this for forward compatibility  */
//! @}

#define __constme		const  c_com_t* const me
#define __rwme			c_com_t* const me

def_interface(i_comEventHandler_t)
	void (*Connected)(int PortID);
	void (*DisConnected)(int PortID);

/*****************************************************************************
* 函 数 名： DataReceived
* 函数功能： 接收到了新数据通知
* 参    数： __constme: 当前对象 
**			 FrameFinishFlag: true:本帧数据结束了；false:本帧数据没结束 
* 返    回： 无
* 描    述：

* 开 发 者：赵明 
* 日    期： 2025-03-26

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	void (*DataReceived)(__constme,bool FrameFinishFlag);
/*****************************************************************************
* 函 数 名： DataSended
* 函数功能： 数据发送完毕通知
* 参    数： __constme: 当前对象 
**			 FrameFinishFlag: true:本帧数据结束了；false:本帧数据没结束 
* 返    回： 无
* 描    述：

* 开 发 者：赵明 
* 日    期： 2025-03-26

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	void (*DataSended)(__constme,bool FrameFinishFlag);
	void (*DataError)(__constme,int errornum);
	void (*SettingCHanged)(__constme,const s_com_Setting_t* const Parm);
end_def_interface(i_comEventHandler_t) /*do not remove this for forward compatibility */


//! \name interface i_com_t
//! @{
def_interface(i_com_t)
	const char* const Name;
	
    e_ComOperationResult_t  (*Init)(__rwme, s_com_init_t *Setting,const i_comEventHandler_t* const EventHandler);
    void  (*DeInit)     (__rwme);
 	e_ComOperationResult_t (*Open)(__constme,void* RunParm);
	e_ComOperationResult_t (*Close)(__constme);
	int (*Read)(__constme,char* ReadTo,int datalength);
	int (*Write)(__constme,const char* DataFrom,int datalength);
/*****************************************************************************
* 函 数 名： ReceiveByteCount
* 函数功能：获取接收到的字节数
* 参    数： __constme: 当前通信口对象
**			 FirstFrameFlag: true-仅获取首帧字节数；false-获取全部字节数 
* 返    回： 负数-异常码；非负数-接收到的字节数
* 描    述：

* 开 发 者：赵明 
* 日    期： 2025-03-26

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	int (*ReceiveByteCount)(__constme,bool FirstFrameFlag);
/*****************************************************************************
* 函 数 名： WriteBufferRemByteCount
* 函数功能：获取发送缓存区剩余字节数
* 参    数： __constme: 当前通信口对象
* 返    回： 负数-异常码；非负数-接收到的字节数
* 描    述：

* 开 发 者：赵明 
* 日    期： 2025-03-26

* 修改历史：
** 1. 新创建

 *****************************************************************************/
 	int (*WriteBufferRemByteCount)(__constme);
	e_COM_State_t (*GetState)(__constme);
/*****************************************************************************
* 函 数 名： PickReceivedStream
* 函数功能：拾取接收到的数据流
* 参    数： __constme: 当前通信口对象
* 参    数： *datalength: 用于保存数据流长度
* 返    回： 数据流指针
* 描    述： 该操作不会将数据流从接收队列中移除

* 开 发 者：赵明 
* 日    期： 2025-03-26

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	char* (*PickReceivedStream)(__constme,int* datalength);
/*****************************************************************************
* 函 数 名： AskWriteBuffer
* 函数功能：申请写入缓存
* 参    数： __constme: 当前通信口对象
* 参    数： *datalength: 用于保存申请到的长度
* 返    回： 缓存指针
* 描    述： 

* 开 发 者：赵明 
* 日    期： 2025-03-26

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	char* (*AskWriteBuffer)(__constme,int* datalength);
	int (*GetSetting)(__constme,s_com_Setting_t* Parm);
	int (*UpdateSetting)(__constme,const s_com_Setting_t* const Parm);
	
  /* other methods */

end_def_interface(i_com_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/

/*============================ PROTOTYPES ====================================*/

#if defined(__com_CLASS_IMPLEMENT__) || defined(__com_CLASS_INHERIT__)

#define COM_OnEvent(__FunName,...)					const i_comEventHandler_t* callback=this->EventHandler;	\
													if (callback && callback->__FunName)	\
													{	\
														callback->__FunName(__VA_ARGS__);	\
													}	\

/*! \brief a method only visible for current class and derived class */
int  COM_Read(const c_com_t* const me,__Out char* ReadTo,int datalength);
int  COM_Write(const c_com_t* const me,__In char* Datap,int datalength);
int  COM_ReceiveByteCount(const c_com_t* const me,bool FirstFrameFlag);
int  COM_WriteBufferRemByteCount(const c_com_t* const me);
char*  COM_PickReceivedStream(const c_com_t* const me,int* datalength);
char*  COM_AskWriteBuffer(const c_com_t* const me,int* datalength);
void COM_ClearBuffer(const c_com_t* const me);
char*  COM_PickWriteStream(const c_com_t* const me,int* datalength);

//驱动回调
int UHC_COM_PacketSended(const c_com_t* const me,__InOut const char** data,__InOut int* datalength);
void UHC_COM_DataReceived(const c_com_t* const me,unsigned int DataLength,bool BufferFulledFlag);
void UHC_COM_Connected(const c_com_t* const me);
void UHC_COM_DisConnected(const c_com_t* const me);
#endif

/*! \note it is very important to undef those macros */
#undef __com_CLASS_INHERIT__
#undef __com_CLASS_IMPLEMENT__

#endif
/* EOF */
