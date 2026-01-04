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


#ifndef __zmSPIBase_H__
#define __zmSPIBase_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>

#include "../zmCOMBase.h"
#include "../../zmGPIO/zmGPIO.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__SPIBase_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__SPIBase_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class SPI_t
//! @{
declare_class(c_SPINode_t)

def_class(c_SPINode_t,  
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

end_def_class(c_SPINode_t) /* do not remove this for forward compatibility  */
//! @}


typedef struct SPINode_cfg_t {
    implement(s_com_Setting_t)
    //! put your configuration members here
    
} s_SPINode_cfg_t;

typedef struct
{
	const s_com_Setting_t* SPIPortConfig;
}s_SPIPortOpenRunParm_t;

//! \name interface i_SPI_t
//! @{
def_interface(i_SPIBase_t)
	implement(i_com_t)
    /* other methods */

end_def_interface(i_SPIBase_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/


/*============================ PROTOTYPES ====================================*/

/*! \note it is very important to undef those macros */
#undef __SPIBase_CLASS_INHERIT__
#undef __SPIBase_CLASS_IMPLEMENT__

#endif
/* EOF */