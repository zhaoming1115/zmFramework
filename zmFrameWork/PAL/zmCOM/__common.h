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


#ifndef __com_H__
#define __com_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../PAL_DataType.h"
#include "../PAL_Config.h"

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
	
}e_ComOperationResult_t;	//串口操作结果类型



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

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Winvalid-source-encoding"
#endif
#define COM_DataLength_OneFrame			Queue_DataLength_OneFrame



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
		const i_comEventHandler_t* EventHandler;
    )
)
end_def_class(c_com_t) /* do not remove this for forward compatibility  */
//! @}

#define __constme		const  c_com_t* const me
#define __rwme			c_com_t* const me

def_interface(i_comEventHandler_t)
	void (*Connected)(int PortID);
	void (*DisConnected)(int PortID);
	void (*DataReceived)(__constme,bool FrameFinishFlag);
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
	int (*ReceiveByteCount)(__constme,bool FirstFrameFlag);
 	int (*WriteBufferRemByteCount)(__constme);
	bool (*IsOpen)(__constme);
	char* (*PickReceivedStream)(__constme,int* datalength);
	char* (*AskWriteBuffer)(__constme,int* datalength);
	int (*GetSetting)(__constme,s_com_Setting_t* Parm);
	int (*UpdateSetting)(__constme,const s_com_Setting_t* const Parm);
	
  /* other methods */

end_def_interface(i_com_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/

/*============================ PROTOTYPES ====================================*/

#if defined(__com_CLASS_IMPLEMENT__) || defined(__com_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void com_protected_method_example(c_com_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __com_CLASS_INHERIT__
#undef __com_CLASS_IMPLEMENT__

#endif
/* EOF */