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
文 件 名： zmStorage.h 
开发日期：2025-03-26 
文件功能：定义存储器对象相关的类。

描    述：存储器管理器对象可统一管理多个不同类型的存储器，其通过地址进行区分。

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/


#ifndef __zmStorage_H__
#define __zmStorage_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../PAL_Config.h"
#include "Sys/Sys_DataType.h"

/*============================ MACROS ========================================*/
#define Storage_WriteAddressForFlush			0			//当写函数传入该地址时，执行FLUSH操作，目的是为了通过一个接口实现存储器的完整写入功能

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef enum
{
	Storage_State_IDLE,
	Storage_State_Busy=!Storage_State_IDLE,
}e_StorageState_t;//存储器状态

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__Storage_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__Storage_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class Storage_t
//! @{
declare_class(c_Storage_t)

def_class(c_Storage_t,

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

end_def_class(c_Storage_t) /* do not remove this for forward compatibility  */
//! @}


typedef struct Storage_cfg_t {
    
    //! put your configuration members here
	unsigned char EWInfoCount;	//擦写信息数量，最小为1，当有多任务同时操作存储器且需要异步擦写时，可以设置更大的数。管理器会自动甄别排序，以确保对同一个存储器的串行操作
} s_Storage_cfg_t;//存储器配置

#define Append_StorageConfig_Default()		{1}

def_interface(i_StorageEvent_t)
	void (*BlockErasing)(unsigned int Address,int Length);						//块将要开始擦除
	void (*BlockErased)(unsigned int Address,int Length);						//块擦除完毕
end_def_interface(i_StorageEvent_t) /*do not remove this for forward compatibility */

//! \name interface i_Storage_t
//! @{
def_interface(i_Storage_t)
    int (*Init)(const s_Storage_cfg_t *const ptCFG,const i_StorageEvent_t* const Event);
	/*函数说明：读取存储设备的数据
	参数说明：
	输入参数：
	Address-存储设备的数据索引地址
	*ToData-要读取到的内存指针
	ByteCount-要读取的字节量
	返回：若ToData为空，则读取到缓存区中，并返回缓存区的地址； 
		否则：>=0-实际读取的数量；<0-异常
	*/
	int (*Read)(unsigned int Address,char* ToData,unsigned int ByteCount);

	/*函数说明：将指定数量的数据写入存储设备
	参数说明：
	输入参数：
	Address-要写入的地址
	*FromData-数据来源内存指针
	ByteCount-要写入的字节量
	返回：>=0-实际写入的数量；<0-异常
	*/
	int (*Write)(unsigned int Address, const char* FromData, unsigned int ByteCount);
	/*函数说明：擦除指定地址所在的扇区或者页
	参数说明：
	输入参数：
	StartAddress-要擦除区域的起始地址
	Length-要擦除区域的字节长度
	返回：>=0-成功；<0-失败
	*/
	int (*Erase)(unsigned int StartAddress,unsigned int Length);

	/*****************************************************************************
	* 函 数 名： IsErased
	* 函数功能： 检查某一段闪存是否需要擦除
	* 参    数： StartAddress-开始地址
	* 参    数： Length-要检查区域的长度，单位：字节,如果为0，则不执行检查任务
	* 参    数： EraseValue-返回擦除后的值，如果为空，则不返回
	* 返    回： int ，需要擦除的字节数
	* 描    述：
	
	* 开 发 者：赵明 
	* 日    期： 2022/08/01
	
	* 修改历史：
	** 1. 新创建
	*****************************************************************************/
	int (*IsNeedErase)(unsigned int* StartAddress,unsigned int Length,char* EraseValue);

	/*函数说明：将临时缓存区中的数据写入存储
	参数说明：
	输入参数：
	Address-要写入的地址
	返回：>=0-实际写入的数量；<0-异常
	*/
	int (*Fulsh)(void);	
    /* other methods */

/*****************************************************************************
* 函 数 名： State
* 函数功能： 获取存储器的状态
* 参    数： Address-存储器地址
* 返    回：e_FlashState_t类型的值
* 描    述： 考虑到FLASH可能分为好几个区，本函数需要指定存储器地址

* 开 发 者：赵明 
* 日    期： 2023-04-14

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	e_StorageState_t (*State)(unsigned int Address);

/*****************************************************************************
* 函 数 名： DisableEWProtect,EnableEWProtect
* 函数功能： 关闭擦写保护，使能擦写保护
* 参    数： Address-存储器地址
* 参    数： Size-保护区大小
* 返    回： 负数-异常码，非负数-实际关联的大小
* 描    述： 

* 开 发 者：赵明 
* 日    期： 2023-04-14

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	bool (*DisableProtect)(unsigned int Address,unsigned int Size,bool RDPFlag);
	bool (*EnableProtect)(unsigned int Address,unsigned int Size);
	bool (*IsEWProtect)(unsigned int Address,unsigned int Size);

end_def_interface(i_Storage_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_Storage_t* const zmStorageManager;

/*============================ PROTOTYPES ====================================*/

#if defined(__Storage_CLASS_IMPLEMENT__) || defined(__Storage_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void Storage_protected_method_example(c_Storage_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __Storage_CLASS_INHERIT__
#undef __Storage_CLASS_IMPLEMENT__

#endif
/* EOF */