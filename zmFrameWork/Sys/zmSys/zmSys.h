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
文 件 名： zmSys.h 
开发日期：2025-03-28 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/


#ifndef __zmSys_H__
#define __zmSys_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../Sys_DataType.h"
#include "../Sys_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__zmSys_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__zmSys_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"

typedef long long tick_t;

//! \name class zmSys_t
//! @{
declare_class(c_zmSys_t)

def_class(c_zmSys_t,

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

end_def_class(c_zmSys_t) /* do not remove this for forward compatibility  */
//! @}


typedef struct zmSys_cfg_t {
    
    //! put your configuration members here
	unsigned short mTickSpan_FeedDog;		//喂狗时间，单位：ms
	unsigned short IntMillSeconds;		//中断毫秒数

} s_zmSys_cfg_t;

//! \name interface i_zmSys_t
//! @{
def_interface(i_zmSys_t)
    int (*Init)(const s_zmSys_cfg_t *ptCFG,unsigned char ClockDivLevel);
	void (*ReStart)(void);	//重启
	void (*EntryLowPower)(unsigned int Level);	
	e_Sys_StartFrom_t (*GetStartFrom)(void);	//获取启动源，返回值见MJ_MCU_StartFromLX
	int (*GetUID)(__Out unsigned char* UIDData);	//获取MCU唯一识别号
	bool (*CHeckUID)(__In unsigned char* UIDData);	//校验MCU唯一识别号
	
	void (*mDelay)(unsigned int MillSecond);	//毫秒延时
	void (*uDelay)(unsigned int MicroSecond);	//微秒延时
	tick_t (*GetmTick)(void);	//获取毫秒时戳
	tick_t (*GetmTickSpan)(tick_t starttick);	//获取与指定时戳的毫秒时戳差
	tick_t (*GetTick)(void);	//获取时戳

	void (*FeedDog)(void);		//喂狗
end_def_interface(i_zmSys_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_zmSys_t* const zmSys;

/*============================ PROTOTYPES ====================================*/

#if defined(__zmSys_CLASS_IMPLEMENT__) || defined(__zmSys_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void zmSys_protected_method_example(c_zmSys_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __zmSys_CLASS_INHERIT__
#undef __zmSys_CLASS_IMPLEMENT__

#endif
/* EOF */