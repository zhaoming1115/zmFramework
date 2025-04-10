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
文 件 名： HAL_Interface_Storage.h 
开发日期：2024-04-14 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/


 #ifndef __H_A_L__INTERFACE__STORAGE_H_
#define __H_A_L__INTERFACE__STORAGE_H_

#define __Storage_StateCHeckOnPAL		1
#ifndef BootProgram
#define Storage_UseBitRW				1
#endif

typedef enum
{
	Storage_Type_ExFLASH=-2,		//外部FLASH
	Storage_Type_ExEPPROM=-1,		//外部EPPROM
	Storage_Type_InnerEPPROM,		//内部EPPROM
	Storage_Type_InnerFlash,		//内部存储器
	Storage_Type_ExFlashAsInner,			//外部存储器，但是可以像内部存储器一样使用
}e_StorageType_t;	//存储器类型

#define Storage_IsWithSysLine(Type)	(Type>=0)			//存储区是否使用系统总线访问，也就是支持地址直访

#define Storage_NoSurportLevel				0xff
#define Storage_IsVaildLevel(__Level)		(__Level>0 && __Level<16)

#if     defined(__BlockStorage_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__BlockStorage_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"

declare_interface(i_StorageBlock_t)

//存储器块对象，每个单独的存储器块都应该实例化一个该对象
declare_class(c_StorageBlock_t)
def_class(c_StorageBlock_t,

    public_member(
        //!< place your public members here
		const i_StorageBlock_t* const Manager;
		e_StorageType_t StorageType;
		char EraseValue;						//擦除后的值,必须为有符号型！
		unsigned char WriteUnitLevel;		//写入单元的大小等级，基2，如：0-1字节,1-2字节，2-4字节。。。。
		unsigned char EraseUnitLevel;		//擦除单元的大小等级，基2，如：0-1字节，1-2字节，2-4字节。。。。
	
		unsigned char ClearBitsLevel;		//清除位数等级，0-1bit，1-2bit，3-1字节，5-4字节，0xff-不支持清除位
		unsigned char ReadbufSizeLevel;		//读缓存区大小等级，基2,0xff-不使用读缓存区
		unsigned char MaxEraseTime;			//擦除一个最小单元需要的最大时间，单位：ms

		unsigned int BaseAddress;
		unsigned int Size;
		
    )

    private_member(
        //!< place your private members here
    )
    
    protected_member(
        //!< place your private members here
    )
)

end_def_class(c_StorageBlock_t) /* do not remove this for forward compatibility  */

//存储器块对象的操作接口，每一类存储器块都应该实现一组该接口
def_interface(i_StorageBlock_t)
    int (*Init)(const c_StorageBlock_t* StorageBlock);
	/*函数说明：读取存储设备的数据
	参数说明：
	输入参数：
	Address-存储设备的数据索引地址
	*ToData-要读取到的内存指针
	ByteCount-要读取的字节量
	返回：若ToData为空，则读取到缓存区中，并返回缓存区的地址； 
		否则：>=0-实际读取的数量；<0-异常
	*/
	int (*Read)(const c_StorageBlock_t* StorageBlock,unsigned int Address,char* ToData,unsigned int ByteCount);

	/*函数说明：将指定数量的数据写入存储设备
	参数说明：
	输入参数：
	Address-要写入的地址
	*FromData-数据来源内存指针
	ByteCount-要写入的字节量
	返回：>=0-实际写入的数量；<0-异常
	*/
	int (*WriteUnit)(const c_StorageBlock_t* StorageBlock,unsigned int Address, const int* FromData);
	/*函数说明：擦除指定地址所在的扇区或者页
	参数说明：
	输入参数：
	Address-要擦除区域的起始地址
	返回：>0：实际擦除的区域大小，<=0:失败
	*/
	int (*EraseUnit)(const c_StorageBlock_t* StorageBlock,unsigned int Address);
	bool (*IsBusy)(const c_StorageBlock_t* StorageBlock,unsigned int Address);

#if Storage_UseBitRW
//	int (*ReadBitUnit)(const c_StorageBlock_t* StorageBlock,unsigned int BitOffsetAddress, int* ToData);
	int (*WriteBitUnit)(const c_StorageBlock_t* StorageBlock,unsigned int BitOffsetAddress, const char* FromData);
#endif
	bool (*DisableProtect)(const c_StorageBlock_t* StorageBlock,unsigned int Address,unsigned int Size,bool RDPFlag);
	bool (*EnableProtect)(const c_StorageBlock_t* StorageBlock,unsigned int Address,unsigned int Size);
	bool (*IsEWProtect)(const c_StorageBlock_t* StorageBlock,unsigned int Address,unsigned int Size);

end_def_interface(i_StorageBlock_t) /*do not remove this for forward compatibility */

extern void Storage_AddDevices(int (*AddStorageBlock)(const c_StorageBlock_t*  StorageBlock));

#undef __BlockStorage_CLASS_INHERIT__
#undef __BlockStorage_CLASS_IMPLEMENT__

#endif


 

