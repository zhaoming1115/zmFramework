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
�� �� ���� zmSerialPort.h 
�������ڣ�2025-03-26 
�ļ����ܣ����崮�ڶ�����ص��ࡣ

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/

#ifndef __SerialPort_H__
#define __SerialPort_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../zmCOMBase.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__SerialPort_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__SerialPort_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class SerialPort_t
//! @{������
declare_class(c_SerialPort_t)

def_class(c_SerialPort_t,  
	which(implement(c_com_t)
		implement(c_ListItem_t)
		)

    public_member(
        //!< place your public members here
    )

    private_member(
        //!< place your private members here
		unsigned short SendingLength;
//		const s_UartAutoContrlParm_t* AutoContrlParm;
    )
    
    protected_member(
        //!< place your private members here
    )
)

end_def_class(c_SerialPort_t) /* do not remove this for forward compatibility  */
//! @}


typedef struct SerialPort_cfg_t {
    implement(s_com_Setting_t)
    //! put your configuration members here
    
} s_SerialPort_cfg_t;//���������������

typedef struct
{
	implement(s_com_init_t)
}s_SerialPort_InitParm_t;//������ĳ�ʼ����������


typedef struct
{
	const s_com_Setting_t* UartConfig;
}s_UartOpenRunParm_t;//�������Open������Ҫ���ݵĲ�������

//! \name interface i_SerialPort_t
//! @{
def_interface(i_SerialPort_t)
	implement(i_com_t)
    /* other methods */
end_def_interface(i_SerialPort_t) /*do not remove this for forward compatibility */
//! @}

//���֡��ʽ������Ϊs_com_Setting_t�����е�TransFormat��Ա��ֵ
#define SerialPort_AppendFrameFormat2(__Parity,__StopBit)		SerialPort_CreateFrameFormatValue(8,__StopBit,__Parity)
#define SerialPort_AppendFrameFormat1(__Parity)					SerialPort_AppendFrameFormat2(SerialPort_StopBit_1,__Parity)
#define SerialPort_AppendFrameFormat(...)						FUN_EVAL(SerialPort_AppendFrameFormat, __VA_ARGS__)(__VA_ARGS__)
#define SerialPort_DefaultFrameFormat				SerialPort_AppendFrameFormat(SerialPort_ParityBit_N)

//��Ӵ������ã�����Ϊs_com_Setting_t���͸�ֵ
#define Append_SerialPort_Setting4(__Baudrate,__ParityBit,__StopBit,__ReceiveMode) \
										{.comType=zmCOM_Type_SerialPort,.TransFormat=SerialPort_AppendFrameFormat(__ParityBit,__StopBit),.ReceiveMode=__ReceiveMode, \
										.SendMode=SerialPort_RWMode_Normal,.Baudrate=__Baudrate}
#define Append_SerialPort_Setting3(__Baudrate,__ParityBit,__StopBit) 	Append_SerialPort_Setting4(__Baudrate,__ParityBit,__StopBit,SerialPort_RWMode_Normal)
#define Append_SerialPort_Setting2(__Baudrate,__ParityBit) 				Append_SerialPort_Setting3(__Baudrate,__ParityBit,SerialPort_StopBit_1)
#define Append_SerialPort_Setting1(__Baudrate)							Append_SerialPort_Setting2(__Baudrate,SerialPort_ParityBit_N)
#define Append_SerialPort_Setting(...)									FUN_EVAL(Append_SerialPort_Setting, __VA_ARGS__)(__VA_ARGS__)


//��ӡ�������ò�����**0-��ӡ��ʽ˵����**1-����ʽ˵����ӡ��ָ����������**2-��ָ���Ĵ������ô�ӡ��ָ���Ļ�����
#define ToString_SerialPort_Setting0()							"##����ͨ�Ų�����ʽ��������,����λ,У��(N-��У��,E-żУ�飬O-��У��),ֹͣλ(1,2,3����1.5λ)���磺115200.8.N,1"
#define ToString_SerialPort_Setting1(__StringBuffer)			sprintf(__StringBuffer,ToString_SerialPort_Setting0())
#define ToString_SerialPort_Setting2(__StringBuffer,__Setting)	({	const u_SerialPortFrameFormat_t* SAFE_NAME(FrameFormat)=(u_SerialPortFrameFormat_t*)&(__Setting)->TransFormat; \
																	sprintf(__StringBuffer,"%d,%d,%c,%d",__Setting->Baudrate,SAFE_NAME(FrameFormat)->DataBit+SerialPort_DataBit_Base,\
																		(SAFE_NAME(FrameFormat)->Parity==SerialPort_ParityBit_E)?'E':(SAFE_NAME(FrameFormat)->Parity==SerialPort_ParityBit_O)?'O':'N', \
																		SAFE_NAME(FrameFormat)->Stop+1);})

#define ToString_SerialPort_Setting(...)						FUN_EVAL(ToString_SerialPort_Setting, __VA_ARGS__)(__VA_ARGS__)

//���ַ�����ʽ�Ĵ������ò�������Ϊ�������ò���ֵ
#define ParseString_SerialPort_Setting2(__String,__Setting)				({int SAFE_NAME(bd)=(__Setting)->Baudrate; \
																		u_SerialPortFrameFormat_t* SAFE_NAME(FrameFormat)=(u_SerialPortFrameFormat_t*)&(__Setting)->TransFormat; \
																		unsigned char SAFE_NAME(db)=SAFE_NAME(FrameFormat)->DataBit+SerialPort_DataBit_Base; \
																		unsigned char SAFE_NAME(sb)=SAFE_NAME(FrameFormat)->Stop+1; \
																		char SAFE_NAME(pb)=(SAFE_NAME(FrameFormat)->Parity==SerialPort_ParityBit_E)?'E':(SAFE_NAME(FrameFormat)->Parity==SerialPort_ParityBit_O)?'O':'N'; \
																		int SAFE_NAME(rst)=sscanf(__String,"%d,%hhu,%c,%hhu",&SAFE_NAME(bd),&SAFE_NAME(db),&SAFE_NAME(pb),&SAFE_NAME(sb));\
																		if(SAFE_NAME(rst)>0){ \
																			(__Setting)->Baudrate=SAFE_NAME(bd); \
																			SAFE_NAME(FrameFormat)->DataBit=SAFE_NAME(db)-SerialPort_DataBit_Base; \
																			SAFE_NAME(FrameFormat)->Stop=SAFE_NAME(sb)-1; \
																			SAFE_NAME(FrameFormat)->Parity=(SAFE_NAME(pb)=='E')?SerialPort_ParityBit_E:(SAFE_NAME(pb)=='O')?SerialPort_ParityBit_O:SerialPort_ParityBit_N;\
																		}\
																		SAFE_NAME(rst);})
#define ParseString_SerialPort_Setting(...)						FUN_EVAL(ParseString_SerialPort_Setting, __VA_ARGS__)(__VA_ARGS__)

//���һ�����ڶ���ʵ��
#define Append_SerialPort(__PortIndex,__Setting)		{.PortIndex=__PortIndex,.Setting=__Setting,.Manager=&SerialPortManager->use_as__i_com_t}

//��ʼ��һ�����ڶ���
#define __Init_SerialPort(__SerialPort,__PortIndex,__Setting,__InitParm,__EventHandler)	 memset(__SerialPort,0,sizeof(c_SerialPort_t)); \
																						with(__SerialPort){ \
																							*(const i_com_t**)(&_->Manager)=&zmSerialPortManager->use_as__i_com_t; \
																							*(char*)(&_->PortIndex)=__PortIndex; \
																							_->InitSetting=__Setting; \
																						} \
																						zmSerialPortManager->Init(&(__SerialPort)->use_as__c_com_t,(__InitParm),__EventHandler)


//ʵ����һ�����ڶ���
#define New_SerialPort4(__PortIndex,__Setting,__InitParm,__EventHandler)		({ c_SerialPort_t* SAFE_NAME(SerialPort)=(c_SerialPort_t*)malloc(sizeof(c_SerialPort_t)); \
																				__Init_SerialPort(SAFE_NAME(SerialPort),__PortIndex,__Setting,__InitParm,__EventHandler); \
																				SAFE_NAME(SerialPort);})

#define New_SerialPort3(__PortIndex,__Setting,__InitParm)		New_SerialPort4(__PortIndex,__Setting,__InitParm,NULL)

#define New_SerialPort2(__PortIndex,__Setting)					New_SerialPort3(__PortIndex,__Setting,NULL)

#define New_SerialPort(...)							FUN_EVAL(New_SerialPort, __VA_ARGS__)(__VA_ARGS__)

//����һ�����ڶ���
#define Dispose_SerialPort(__SerialPort)		zmSerialPortManager->DeInit(__SerialPort); \
												free(__SerialPort)


/*============================ GLOBAL VARIABLES ==============================*/
extern const i_SerialPort_t* const zmSerialPortManager;

/*============================ PROTOTYPES ====================================*/


#if defined(__SerialPort_CLASS_IMPLEMENT__) || defined(__SerialPort_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
#endif

/*! \note it is very important to undef those macros */
#undef __SerialPort_CLASS_INHERIT__
#undef __SerialPort_CLASS_IMPLEMENT__

#endif
/* EOF */