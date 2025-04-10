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
文 件 名： zmFrameQueue.h 
开发日期：2025-03-28 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/



#ifndef __FrameQueue_H__
#define __FrameQueue_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>

#include "zmQueue.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__FrameQueue_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__FrameQueue_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class FrameQueue_t
//! @{
declare_class(c_FrameQueue_t)

def_class(c_FrameQueue_t,  which(implement(c_Queue_t))

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

end_def_class(c_FrameQueue_t) /* do not remove this for forward compatibility  */
//! @}


typedef struct FrameQueue_cfg_t {
    
    //! put your configuration members here
    
} s_FrameQueue_cfg_t;

//! \name interface i_FrameQueue_t
//! @{
def_interface(i_FrameQueue_t)
	implement(i_Queue_t)
    int (*FirstFrameLength) (c_FrameQueue_t *Queue);
    /* other methods */

end_def_interface(i_FrameQueue_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_FrameQueue_t* const zmFrameQueueManager;

/*============================ PROTOTYPES ====================================*/


/*! \note it is very important to undef those macros */
#undef __FrameQueue_CLASS_INHERIT__
#undef __FrameQueue_CLASS_IMPLEMENT__

#endif
/* EOF */