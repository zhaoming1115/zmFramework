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
文 件 名： zmGPIO.h 
开发日期：2025-03-26 
文件功能：定义GPIO对象相关的类。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/


#ifndef __GPIO_H__
#define __GPIO_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../PAL_DataType.h"
#include "../PAL_Config.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__GPIO_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__GPIO_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class GPIO_t
//! @{
declare_class(c_GPIO_t)

def_class(c_GPIO_t,

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

end_def_class(c_GPIO_t) /* do not remove this for forward compatibility  */
//! @}


typedef void (*f_GPIOInputCHanged)(void);

//! \name interface i_GPIO_t
//! @{
def_interface(i_GPIO_t)
	int (*Init)(unsigned char SurportExITCount);
	int (*SetMode)(gpio_t GPIOx,e_GPIO_Mode_t mode);
	e_GPIO_Value_t (*ReadPin)(gpio_t GPIOx);
	int (*WritePin)(gpio_t GPIOx,e_GPIO_Value_t GPIO_Value_CHange);

/*****************************************************************************
* 函 数 名： SetExitIT
* 函数功能：设置外部中断
* 参    数： GPIOx: 外部中断对应的GPIO编号 
**			 TriggerMode: 触发模式 
**			 CallBack: 发生中断后的回调函数，若为空，则调用通用的中断处理函数 
* 返    回： 0-成功，非0-异常码
* 描    述：

* 开 发 者：赵明 
* 日    期： 2025-03-26

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	int (*SetExitIT)(gpio_t GPIOx,e_Exit_TriggerMode_t TriggerMode,f_GPIOInputCHanged CallBack);
/*****************************************************************************
* 函 数 名： ClearExitIT
* 函数功能：清除外部中断
* 参    数： GPIOx: 外部中断对应的GPIO编号 
* 返    回： 0-成功，非0-异常码
* 描    述：

* 开 发 者：赵明 
* 日    期： 2025-03-26

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	int (*ClearExitIT)(gpio_t GPIOx);
    /* other methods */

end_def_interface(i_GPIO_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_GPIO_t* const zmGPIOManager;

/*============================ PROTOTYPES ====================================*/

#if defined(__GPIO_CLASS_IMPLEMENT__) || defined(__GPIO_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void GPIO_protected_method_example(c_GPIO_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __GPIO_CLASS_INHERIT__
#undef __GPIO_CLASS_IMPLEMENT__

#endif
/* EOF */
