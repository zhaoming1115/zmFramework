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
文 件 名： zmList.c 
开发日期：2025-03-28 
文件功能：实现列表管理器对象

描    述：

依赖接口：

提供接口：zmListManager对象
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/


/*============================ INCLUDES ======================================*/
#define __List_CLASS_IMPLEMENT__

#include "zmList.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)    assert(__VA_ARGS__)
#endif

#undef this
#define this        (ptThis)

/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

/*============================ IMPLEMENTATION ================================*/
static
c_List_t* List_Init(c_List_t *me)
{
	me->Head=NULL;
	return me;
}

static
void  List_DeInit(c_List_t *me)
{
	c_ListItem_t* _Item=me->Head;
	while(_Item)
	{
		c_ListItem_t* Next=_Item->Next;
		_Item->Next=NULL;
		_Item=Next;
	}
}

static
void List_Add(c_List_t *me,c_ListItem_t* item)
{
	c_ListItem_t* _Item=me->Head;
	if(me->Head==NULL)
	{
List_SafeCode	{
					me->Head=item;
					item->Next=NULL;
				}
	}
	else
	{
		while(_Item->Next)
		{
			_Item=_Item->Next;
		}
List_SafeCode	{
					_Item->Next=item;
					item->Next=NULL;
				}
	}
}

static
void List_Remove(c_List_t *me,c_ListItem_t* item)
{
	if(me->Head==item)
	{
List_SafeCode	{
					me->Head=me->Head->Next;
//					me->Head->Next=NULL;
				}
	}
	else
	{
		c_ListItem_t* _Item=me->Head;
		while(_Item->Next)
		{
			if(_Item->Next==item)
			{
List_SafeCode	{
					_Item->Next=_Item->Next->Next;
//					_Item->Next->Next=NULL;
				}
				break;
			}
			else
			{
				_Item=_Item->Next;
			}
		}
	}
}

c_ListItem_t* List_Find(c_List_t *me,const void* FindObject,bool (*CHeckFun)(c_ListItem_t* item,const void* FindObject))
{
	c_ListItem_t* Item=me->Head;
	while(Item)
	{
		if(CHeckFun(Item,FindObject)) return Item;
		Item=Item->Next;
	}
	return NULL;
}

static
bool List_Contains(c_List_t *me,c_ListItem_t* item)
{
	const c_ListItem_t* _Item=me->Head;
	while(_Item)
	{
		if(_Item==item) return true;
		_Item=_Item->Next;
	}
	return false;
}

static
c_ListItem_t* List_Next(c_List_t* me,c_ListItem_t* item)
{
	return (item)?item->Next:NULL;
}

static
c_ListItem_t* List_First(c_List_t* me)
{
	return me->Head;
}


const i_List_t* const zmListManager = &(const i_List_t){
    .Init =&List_Init,
    .DeInit = &List_DeInit,
    .Add=&List_Add,
	.Remove=&List_Remove,
	.Find=&List_Find,
	.Contains=&List_Contains,
	.First=&List_First,
	.Next=&List_Next
    /* other methods */
};


/* EOF */