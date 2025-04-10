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
文 件 名： zmNet.h 
开发日期：2025-04-09 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __ZM_NET_H_
#define __ZM_NET_H_
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
   
#if     defined(__Net_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__Net_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class Net_t
//! @{
declare_class(c_Net_t)

def_class(c_Net_t, 
	which(implement(c_com_t)
			implement(c_ListItem_t)
		)


    public_member(
        //!< place your public members here
    )

    private_member(
        //!< place your private members here
    )
    
    protected_member(
        //!< place your private members here
    )
)

end_def_class(c_Net_t) /* do not remove this for forward compatibility  */
//! @}


typedef struct Net_cfg_t {
    implement(s_com_Setting_t)
    
    //! put your configuration members here
    
} s_Net_cfg_t;

//! \name interface i_Net_t
//! @{
def_interface(i_Net_t)
	implement(i_com_t)
    /* other methods */

end_def_interface(i_Net_t) /*do not remove this for forward compatibility */
//! @}

#define __Net_ParseSubIPAddr(__IPString,Index)	((__IPString[Index+0]-'0')*100+(__IPString[Index+1]-'0')*10+(__IPString[Index+2]-'0'))
#define Net_ParseIPAddr(__IPString)				{__Net_ParseSubIPAddr(__IPString,0),__Net_ParseSubIPAddr(__IPString,4),__Net_ParseSubIPAddr(__IPString,8),__Net_ParseSubIPAddr(__IPString,12)}

#define Append_Net_Setting3(__TransFormat,__IPAddr,__PortNum) 	{.comType=zmCOM_Type_Net,.TransFormat=__TransFormat, .NetPort=__PortNum,\
																		.IPAddr=Net_ParseIPAddr(__IPAddr)}
#define Append_Net_Setting2(__IPAddr,__PortNum) 					Append_Net_Setting3(Net_FrameFormat_TCP,__IPAddr,__PortNum)
#define Append_Net_Setting1(__IPAddr)							Append_Net_Setting2(__IPAddr,0)
#define Append_Net_Setting(...)									FUN_EVAL(Append_Net_Setting, __VA_ARGS__)(__VA_ARGS__)


#define ToString_Net_Setting0()							"##网络通信参数格式：通信机制(TCP|UDP),IP地址:端口号，如：TCP,111.122.133.144:56789"
#define ToString_Net_Setting1(__StringBuffer)			sprintf(__StringBuffer,ToString_Net_Setting0())
#define ToString_Net_Setting2(__StringBuffer,__Setting)	({	const u_NetFrameFormat_t* SAFE_NAME(FrameFormat)=(u_NetFrameFormat_t*)&(__Setting)->TransFormat; \
																	sprintf(__StringBuffer,"%s,%03hhu.%03hhu.%03hhu.%03hhu:%hu",(SAFE_NAME(FrameFormat)->FrameFormat==Net_FrameFormat_TCP)?"TCP":"UDP",\
																	(__Setting)->IPAddr[0],(__Setting)->IPAddr[1],(__Setting)->IPAddr[2],(__Setting)->IPAddr[3],(__Setting)->NetPort);\
																	})

#define ToString_Net_Setting(...)						FUN_EVAL(ToString_Net_Setting, __VA_ARGS__)(__VA_ARGS__)

#define ParseString_Net_Setting2(__String,__Setting)				({int SAFE_NAME(rst)=-1; \
																		u_NetFrameFormat_t* SAFE_NAME(FrameFormat)=(u_NetFrameFormat_t*)&(__Setting)->TransFormat; \
																		if(StringIsSame(__String,"TCP",3)){ \
																			SAFE_NAME(FrameFormat)->FrameFormat=Net_FrameFormat_TCP; \
																			SAFE_NAME(rst)++; \
																		} else if(StringIsSame(__String,"UDP",3)){ \
																			SAFE_NAME(FrameFormat)->FrameFormat=Net_FrameFormat_UDP; \
																			SAFE_NAME(rst)++; \
																		} \
																		if(SAFE_NAME(rst)==0) { \
																			SAFE_NAME(rst)=sscanf((__String)+4,"%03hhu.%03hhu.%03hhu.%03hhu:%hu",&(__Setting)->IPAddr[0],&(__Setting)->IPAddr[1], \
																										&(__Setting)->IPAddr[2],&(__Setting)->IPAddr[3],&(__Setting)->NetPort);\
																			if(SAFE_NAME(rst)!=5){ \
																				SAFE_NAME(rst)=-1; \
																			}\
																		}\
																		SAFE_NAME(rst);})
#define ParseString_Net_Setting(...)									FUN_EVAL(ParseString_Net_Setting, __VA_ARGS__)(__VA_ARGS__)

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_Net_t* const zmNetManager;

/*============================ PROTOTYPES ====================================*/

#if defined(__Net_CLASS_IMPLEMENT__) || defined(__Net_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
#endif

/*! \note it is very important to undef those macros */
#undef __Net_CLASS_INHERIT__
#undef __Net_CLASS_IMPLEMENT__

#endif
/* EOF */