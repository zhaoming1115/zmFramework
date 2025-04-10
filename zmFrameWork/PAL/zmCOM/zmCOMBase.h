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
�� �� ���� zmCOMBase.h 
�������ڣ�2025-03-26 
�ļ����ܣ�Ϊ����ͨ����˿��ṩһ��ͳһ�Ľӿڡ�

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

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
	COM_OPRST_MallocFail,		//�����ڴ�ʧ��
	COM_OPRST_ParmFail,		//����Ĳ����쳣
	COM_OPRST_MemoryIsUsed,		//�ڴ�ռ��
	COM_OPRST_PortIsBusy,		//�˿�æ
	COM_OPRST_ConfigFail,		//����ʧ��
	COM_OPRST_NoEnoughMemory,		//û���㹻���ڴ�
	COM_OPRST_LoadSendDataFailed,		//���ط�������ʧ��
	COM_OPRST_GetRxDataFailed,		//��ȡ���յ�������ʧ��
	COM_OPRST_DataIsReceiving,	//�������ڽ���
	COM_OPRST_PortNotInLine,	//�˿ڲ����ߣ����������USB���Ȳ�εĽӿ�
	COM_OPRST_NoSurport,	//��֧�ֵĲ���
	
}e_ComOperationResult_t;	//���пڲ����������

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

#define COM_DataLength_OneFrame			Queue_DataLength_OneFrame			//���ݳ���Ϊ1֡



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
* �� �� ���� DataReceived
* �������ܣ� ���յ���������֪ͨ
* ��    ���� __constme: ��ǰ���� 
**			 FrameFinishFlag: true:��֡���ݽ����ˣ�false:��֡����û���� 
* ��    �أ� ��
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2025-03-26

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	void (*DataReceived)(__constme,bool FrameFinishFlag);
/*****************************************************************************
* �� �� ���� DataSended
* �������ܣ� ���ݷ������֪ͨ
* ��    ���� __constme: ��ǰ���� 
**			 FrameFinishFlag: true:��֡���ݽ����ˣ�false:��֡����û���� 
* ��    �أ� ��
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2025-03-26

* �޸���ʷ��
** 1. �´���

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
* �� �� ���� ReceiveByteCount
* �������ܣ���ȡ���յ����ֽ���
* ��    ���� __constme: ��ǰͨ�ſڶ���
**			 FirstFrameFlag: true-����ȡ��֡�ֽ�����false-��ȡȫ���ֽ��� 
* ��    �أ� ����-�쳣�룻�Ǹ���-���յ����ֽ���
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2025-03-26

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	int (*ReceiveByteCount)(__constme,bool FirstFrameFlag);
/*****************************************************************************
* �� �� ���� WriteBufferRemByteCount
* �������ܣ���ȡ���ͻ�����ʣ���ֽ���
* ��    ���� __constme: ��ǰͨ�ſڶ���
* ��    �أ� ����-�쳣�룻�Ǹ���-���յ����ֽ���
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2025-03-26

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
 	int (*WriteBufferRemByteCount)(__constme);
	e_COM_State_t (*GetState)(__constme);
/*****************************************************************************
* �� �� ���� PickReceivedStream
* �������ܣ�ʰȡ���յ���������
* ��    ���� __constme: ��ǰͨ�ſڶ���
* ��    ���� *datalength: ���ڱ�������������
* ��    �أ� ������ָ��
* ��    ���� �ò������Ὣ�������ӽ��ն������Ƴ�

* �� �� �ߣ����� 
* ��    �ڣ� 2025-03-26

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	char* (*PickReceivedStream)(__constme,int* datalength);
/*****************************************************************************
* �� �� ���� AskWriteBuffer
* �������ܣ�����д�뻺��
* ��    ���� __constme: ��ǰͨ�ſڶ���
* ��    ���� *datalength: ���ڱ������뵽�ĳ���
* ��    �أ� ����ָ��
* ��    ���� 

* �� �� �ߣ����� 
* ��    �ڣ� 2025-03-26

* �޸���ʷ��
** 1. �´���

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

//�����ص�
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
