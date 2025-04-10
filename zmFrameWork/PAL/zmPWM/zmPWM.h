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
文 件 名： zmPWM.h 
开发日期：2025-04-09 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __ZM_P_W_M_H_
#define __ZM_P_W_M_H_
/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__PWM_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__PWM_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class PWM_t
//! @{
declare_class(c_PWM_t)

def_class(c_PWM_t,

    public_member(
        //!< place your public members here
		unsigned char CHannelIndex;
    )

    private_member(
        //!< place your private members here
    )
    
    protected_member(
        //!< place your private members here
    )
)

end_def_class(c_PWM_t) /* do not remove this for forward compatibility  */
//! @}


typedef struct PWM_cfg_t {
    
    //! put your configuration members here
    unsigned int PinLv;
	unsigned int JingDu;
} s_PWM_cfg_t;

//! \name interface i_PWM_t
//! @{
def_interface(i_PWM_t)
    int  (*Init)(c_PWM_t *ptObj, const s_PWM_cfg_t *ptCFG);
    void (*DeInit)(c_PWM_t *ptObj);
	void (*Start)(c_PWM_t *ptObj);
	void (*Stop)(c_PWM_t *ptObj);
	void (*SetMaiKuan)(c_PWM_t *ptObj,unsigned int WanFenBi);
	unsigned int (*GetMaiKuan)(c_PWM_t *ptObj);
	void (*SetMaiKuanWithTick)(c_PWM_t *ptObj,unsigned int Tick);
	unsigned int (*GetMaiKuanWithTick)(c_PWM_t *ptObj);
	
    /* other methods */

end_def_interface(i_PWM_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_PWM_t* const zmPWMManager;

/*============================ PROTOTYPES ====================================*/

#if defined(__PWM_CLASS_IMPLEMENT__) || defined(__PWM_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void PWM_protected_method_example(c_PWM_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __PWM_CLASS_INHERIT__
#undef __PWM_CLASS_IMPLEMENT__

#endif
/* EOF */