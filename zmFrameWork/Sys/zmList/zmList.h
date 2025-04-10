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
文 件 名： zmList.h 
开发日期：2025-03-28 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/



#ifndef __List_H__
#define __List_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../Sys_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__List_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__List_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class List_t
//! @{
declare_class(c_ListItem_t)

def_class(c_ListItem_t,

    public_member(
        //!< place your public members here
    )

    private_member(
        //!< place your private members here
    )
    
    protected_member(
        //!< place your private members here
		c_ListItem_t* Next;
    )
)
end_def_class(c_ListItem_t) /* do not remove this for forward compatibility  */

declare_class(c_List_t)

def_class(c_List_t,

    public_member(
        //!< place your public members here
    )

    private_member(
        //!< place your private members here
    )
    
    protected_member(
        //!< place your private members here
		c_ListItem_t* Head;
    )
)

end_def_class(c_List_t) /* do not remove this for forward compatibility  */
//! @}

#define List_ItemForInheritObject(__Item,__InheritObjectType)			((__Item==NULL)?NULL: \
																		(__InheritObjectType*)((unsigned int)__Item-offsetof(__InheritObjectType,use_as__c_ListItem_t)))
//! \name interface i_List_t
//! @{
def_interface(i_List_t)
    c_List_t* (*Init)(c_List_t *me);
    void  (*DeInit)(c_List_t *me);
	void (*Add)(c_List_t *me,c_ListItem_t* item);
	void (*Remove)(c_List_t *me,c_ListItem_t* item);
	c_ListItem_t* (*Find)(c_List_t *me,const void* FindObject,bool (*CHeckFun)(c_ListItem_t* item,const void* FindObject));
	bool (*Contains)(c_List_t *me,c_ListItem_t* item);
	c_ListItem_t* (*Next)(c_List_t* me,c_ListItem_t* item);
	c_ListItem_t* (*First)(c_List_t* me);
end_def_interface(i_List_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_List_t* const zmListManager;

/*============================ PROTOTYPES ====================================*/

#if defined(__List_CLASS_IMPLEMENT__) || defined(__List_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void List_protected_method_example(c_List_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __List_CLASS_INHERIT__
#undef __List_CLASS_IMPLEMENT__

#endif
/* EOF */