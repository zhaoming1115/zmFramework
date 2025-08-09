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


#ifndef __zmI2C_H__
#define __zmI2C_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>

#include "../zmComBase.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef enum I2C_AskTransFlag_t {
    
    //! put your configuration members here
    I2C_ATFlag_None=0,
    I2C_ATFlag_Read=1<<0,
    I2C_ATFlag_Write=1<<1,

} e_I2C_AskTransFlag_t;

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__I2C_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__I2C_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class I2C_t
//! @{

declare_class(c_I2CNode_t)

def_class(c_I2CNode_t,  	
which(implement(c_com_t)
		implement(c_ListItem_t)
		)


    public_member(
        //!< place your public members here
		
    )

    private_member(
        //!< place your private members here
		unsigned short SendingLength;
		unsigned char AskTransFlag;		//1-Read,2-Send
    )
    
    protected_member(
        //!< place your private members here
    )
)

end_def_class(c_I2CNode_t) /* do not remove this for forward compatibility  */

typedef struct I2C_cfg_t {
    
    //! put your configuration members here
    
} s_I2C_cfg_t;

//! \name interface i_I2C_t
//! @{
def_interface(i_I2C_t)
	implement(i_com_t)
    /* other methods */

end_def_interface(i_I2C_t) /*do not remove this for forward compatibility */
//! @}

#define I2CNode_AppendTransFormat2(__MasterFlag,__Addr16Flag)		I2C_CreateTransFormatValue(__MasterFlag,__Addr16Flag)
#define I2CNode_AppendTransFormat0()								I2CNode_AppendTransFormat2(1,0)
#define I2CNode_AppendTransFormat(...)								FUN_EVAL(I2CNode_AppendTransFormat, __VA_ARGS__)(__VA_ARGS__)
#define I2CNode_DefaultTransFormat									I2CNode_AppendTransFormat()

#define Append_I2CNode_Setting4(__NodeAddr,__Baudrate,__MasterFlag,__Addr16Flag) \
										{.comType=zmCOM_Type_I2C,.TransFormat=I2CNode_AppendTransFormat(__MasterFlag,__Addr16Flag),.I2CNodeAddr=__NodeAddr,.Baudrate=__Baudrate}
#define Append_I2CNode_Setting3(__NodeAddr,__Baudrate,__MasterFlag) 	Append_I2CNode_Setting4(__NodeAddr,__Baudrate,__MasterFlag,0)
#define Append_I2CNode_Setting2(__NodeAddr,__Baudrate) 					Append_I2CNode_Setting3(__NodeAddr,__Baudrate,1)
#define Append_I2CNode_Setting1(__NodeAddr)								Append_I2CNode_Setting2(__NodeAddr,100000)
#define Append_I2CNode_Setting(...)										FUN_EVAL(Append_I2CNode_Setting, __VA_ARGS__)(__VA_ARGS__)

#define __Init_I2CNode(__I2CNode,__PortIndex,__Setting,__InitParm,__EventHandler)	 memset(__I2CNode,0,sizeof(c_I2CNode_t)); \
																						with(__I2CNode){ \
																							*(const i_com_t**)(&_->Manager)=&zmI2CManager->use_as__i_com_t; \
																							*(char*)(&_->PortIndex)=zmCOM_CreatePortID(__PortIndex,zmCOM_Type_I2C); \
																							_->InitSetting=__Setting; \
																						} \
																						zmI2CManager->Init(&__I2CNode->use_as__c_com_t,(__InitParm),__EventHandler)


//#define New_I2CNode3(__PortIndex,__Baudrate,__ParityBit)		({ c_I2CNode_t* SAFE_NAME(I2CNode)=(c_I2CNode_t*)malloc(sizeof(c_I2CNode_t)+sizeof(s_I2CNode_cfg_t)); \
//																	s_I2CNode_cfg_t* SAFE_NAME(I2CNodeCfg)=(s_I2CNode_cfg_t*)(SAFE_NAME(I2CNode)+1); \
//																	const s_I2CNode_cfg_t SAFE_NAME(I2CNodeCfg0)=Append_I2CNode_Setting(__Baudrate,__ParityBit); \
//																	memcpy(SAFE_NAME(I2CNodeCfg),SAFE_NAME(I2CNodeCfg0),sizeof(s_I2CNode_cfg_t)); \
//																	__Init_I2CNode(SAFE_NAME(I2CNode),__PortIndex,SAFE_NAME(I2CNodeCfg)); \
//																	SAFE_NAME(I2CNode);})

#define New_I2CNode4(__PortIndex,__Setting,__InitParm,__EventHandler)		({ c_I2CNode_t* SAFE_NAME(I2CNode)=(c_I2CNode_t*)malloc(sizeof(c_I2CNode_t)); \
																				__Init_I2CNode(SAFE_NAME(I2CNode),__PortIndex,__Setting,__InitParm,__EventHandler); \
																				SAFE_NAME(I2CNode);})

#define New_I2CNode3(__PortIndex,__Setting,__InitParm)		New_I2CNode4(__PortIndex,__Setting,__InitParm,NULL)

#define New_I2CNode2(__PortIndex,__Setting)					New_I2CNode3(__PortIndex,__Setting,NULL)

#define New_I2CNode(...)							FUN_EVAL(New_I2CNode, __VA_ARGS__)(__VA_ARGS__)

#define Dispose_I2CNode(__I2CNode)		zmI2CNodeManager->DeInit(__I2CNode); \
												free(__I2CNode)

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_I2C_t* const zmI2CManager;

/*============================ PROTOTYPES ====================================*/

#if defined(__I2C_CLASS_IMPLEMENT__) || defined(__I2C_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
#endif

/*! \note it is very important to undef those macros */
#undef __I2C_CLASS_INHERIT__
#undef __I2C_CLASS_IMPLEMENT__

#endif
/* EOF */