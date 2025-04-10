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
文 件 名： zmElementInfo.h 
开发日期：2025-04-09 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __ZM_ELEMENT_INFO_H_
#define __ZM_ELEMENT_INFO_H_

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../Sys_cfg.h"
#include "../Sys_DataType.h"

/*============================ MACROS ========================================*/
#define Element_UnknownAddr				0xffffffffu

/*============================ MACROFIED FUNCTIONS ===========================*/
#define __Element_GetTypeID(__Element)                                               \
            _Generic((__Element),                                                   \
              const unsigned char: DataType_Uint8,                        \
                    unsigned char: DataType_Uint8,                        \
					  const char: DataType_int8,                        \
							char: DataType_int8,                        \
             const unsigned short: DataType_Uint16,                          \
                  unsigned short: DataType_Uint16,                          \
					const short: DataType_int16,                         \
							short: DataType_int16,                         \
				const unsigned int: DataType_Uint32,                         \
					unsigned int: DataType_Uint32,                         \
						const int: DataType_int32,                        \
							 int: DataType_int32,                        \
		const unsigned long long: DataType_Uint64,                         \
			unsigned long long: DataType_Uint64,                         \
				const long long: DataType_int64,                        \
					long long: DataType_int64,                        \
                  const float   : DataType_float32,                         \
                        float   : DataType_float32,                         \
                  const double  : DataType_float64,                         \
                        double  : DataType_float64,                         \
           const unsigned char*  : sizeof(__Element),                         \
                  unsigned char*  : sizeof(__Element),                         \
                    const char*  : (sizeof(__Element)>=128)?-128:-(char)sizeof(__Element), \
                       char*  :(sizeof(__Element)>=128)?-128:-(char)sizeof(__Element), \
					default: DataType_Fun \
                   )
#define Element_GetTypeID(__Element)          __Element_GetTypeID(__Element)

/*============================ TYPES =========================================*/
typedef enum
{
	Element_CHeckRst_NoElement=-1,
	Element_CHeckRst_OK,
	Element_CHeckRst_InfoCHanged,
	
}e_ElementInfoCHeckResult_t;

typedef int (*f_Element_Fun_t)(int ArrayIndex,const char* ParmString,const char** Errdesc);
typedef struct
{
//	const char Name[20];	//元素名称，若元素为双字，则名称的后8个字节代表有效值的上限，此时名称最多存储11个字节的名称，必须保证字符串的最后1个字节为0
#pragma pack(4)
	unsigned int ElementAddr;
#pragma pack()
#pragma pack(1)
	char Type;		//见 e_BaseDataType_t
	union
	{
		unsigned char ArrayInfo;	//数组信息，
		struct
		{
			unsigned char ElementCount:4;		//元素数量，0代表1，依次类推
			unsigned char ElementSpanWords:4;		//元素间隔字数，若为0，则代表元素为连续存放，否则代表两个元素所在地址之差的字数
		};
	};
#pragma pack()
	union
	{
		char Name[18];	//元素名称，若元素为双字，则名称的后8个字节代表有效值的上限，此时名称最多存储11个字节的名称，必须保证字符串的最后1个字节为0
		struct
		{
			char DoubleTypeName[9];		//双字元素名称
			char DoubleNameEnd;		//名称字符串结尾
			long long DoubleMaxValue;		//双字元素检查值的上限
		};
		struct
		{
			char SingleTypeName[17];		//单字元素名称
			char SingleNameEnd;		//名称字符串结尾
		};		
	};
	union
	{
		long long DoubleMinValue;	//双字元素检查值的下限，
		struct
		{
			int MinValue;
			int MaxValue;
		};
	};

}s_ElementInfo_t;		//元素信息类型，用于描述一个元素并设定一个检查范围

#define Element_IsDoubleType(__Type)					(((__Type)==DataType_int64 || (__Type)==DataType_Uint64  || (__Type)==DataType_float64))
#define Element_IsStreamType(__Type)					((__Type<0) || (__Type<DataType_ByteArrayBase))
#define Element_IsStream(__Element)						(sizeof(__Element)>8)
#define Element_IsDouble(__Element)						(sizeof(__Element)==8)

#define Append_ElementInfo6_64(__Name,__MinValue,__MaxValue)	.DoubleTypeName={__Name},.DoubleNameEnd=0,.DoubleMaxValue=__MaxValue,.DoubleMinValue=__MinValue


#define Append_ElementInfo6_32(__Name,__MinValue,__MaxValue)	.SingleTypeName={__Name},.SingleNameEnd=0,.MinValue=(__MinValue),.MaxValue=__MaxValue

//#define Append_ElementInfo6(__Name,__Element,__MinValue,__MaxValue,__ElementCount,__Span) \
//				(Element_IsDouble(__Element) ? Append_ElementInfo6_64(__Element_GetTypeID(__Element),__Name,__Element,__MinValue,__MaxValue,__ElementCount,__Span) : \
//				Append_ElementInfo6_32(__Element_GetTypeID(__Element),__Name,__Element,Element_IsStream(__Element)?sizeof(__Element):__MinValue,__MaxValue,__ElementCount,__Span))
//#define __Append_ElementInfo6(__Name,__Element,__MinValue,__MaxValue,__ElementCount,__Span) \
//            _Generic((__Element),                                                   \
//               unsigned char: Append_ElementInfo6_32(DataType_Uint8,__Name,__Element,__MinValue,__MaxValue,__ElementCount,__Span),      \
//                       char*  : Append_ElementInfo6_32(-(char)sizeof(__Element),__Name,__Element,sizeof(__Element),__MaxValue,__ElementCount,__Span) \
//                   )

//#define Append_ElementInfo6(__Name,__Element,__MinValue,__MaxValue,__ElementCount,__Span) 	__Append_ElementInfo6(__Name,__Element,__MinValue,__MaxValue,__ElementCount,__Span)
//Element_IsDoubleType(Element_GetTypeID(__Element))?
//																							Append_ElementInfo6_64(__Name,__MinValue,__MaxValue), : \
																							
#define Append_ElementInfo7(__Name,__Element,__ElementAddr,__MinValue,__MaxValue,__ElementCount,__Span) {Append_ElementInfo6_32(__Name,__MinValue,__MaxValue),\
																								.ElementAddr=__ElementAddr, .Type=Element_GetTypeID(__Element),\
																								.ElementCount=(__ElementCount)-1,.ElementSpanWords=(__Span)>>2}

#define Append_ElementInfo6(__Name,__Element,__MinValue,__MaxValue,__ElementCount,__Span) \
											Append_ElementInfo7(__Name,__Element,(unsigned int)&__Element,Element_IsStream(__Element)?sizeof(__Element):__MinValue,__MaxValue,__ElementCount,__Span)

#define Append_ElementInfo5(__Name,__Element,__ElementAddr,__MinValue,__MaxValue)		Append_ElementInfo7(__Name,__Element,__ElementAddr, \
																						Element_IsStream(__Element)?sizeof(__Element):__MinValue,__MaxValue,1,0)
#define Append_ElementInfo4(__Name,__Element,__MinValue,__MaxValue)						Append_ElementInfo6(__Name,__Element,0,0,1,0)
#define Append_ElementInfo3(__Name,__Element,__ElementAddr)								Append_ElementInfo5(__Name,__Element,__ElementAddr,0,0)
#define Append_ElementInfo2(__Name,__Element)											Append_ElementInfo4(__Name,__Element,0,0)
#define Append_ElementInfo0()															Append_ElementInfo2(NULL,0)
#define Append_ElementInfo(...)															FUN_EVAL(Append_ElementInfo, __VA_ARGS__)(__VA_ARGS__)
#define Append_ElementInfo_End()														Append_ElementInfo0(NULL,0)

inline static bool Element_InfoIsValid(const s_ElementInfo_t* Info)
{
	return (Info->Name[0]!=0 && Info->Name[0]!=-1);
}

inline static bool Element_IsArray(const s_ElementInfo_t* Info)
{
	return Info->ElementCount;
}

inline static int Element_ArrayCount(const s_ElementInfo_t* Info)
{
	return Info->ElementCount+1;
}

inline static bool Element_IsVaildArrayIndex(const s_ElementInfo_t* Info,int Index)
{
	return Info->ElementCount>=Index;
}

//inline static int Element_GetArrayIncLength(const s_ElementInfo_t* Info)
//{
//	return (Info->ElementSpanWords)?(Info->ElementSpanWords<<2):Sys_GetDataTypeSize(Info->Type);
//}

inline static int Element_GetOffsetAddressWithArrayBase(const s_ElementInfo_t* Info,int Index)
{
	int Span=(Info->Type==DataType_Fun)?0:
			(Info->ElementSpanWords)?Info->ElementSpanWords<<2:Sys_GetDataTypeSize(Info->Type);
	return  Span*Index;
}

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__ElementInfo_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__ElementInfo_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class ElementInfo_t
//! @{
declare_class(c_ElementInfo_t)

def_class(c_ElementInfo_t,

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

end_def_class(c_ElementInfo_t) /* do not remove this for forward compatibility  */
//! @}


typedef struct ElementInfo_cfg_t {
    
    //! put your configuration members here
    
} s_ElementInfo_cfg_t;

def_interface(i_ElementInfoEvent_t)
    e_ElementInfoCHeckResult_t  (*CHeckElement)(s_ElementInfo_t* ElementInfo);
    /* other methods */

end_def_interface(i_ElementInfoEvent_t) /*do not remove this for forward compatibility */

//! \name interface i_ElementInfo_t
//! @{
def_interface(i_ElementInfo_t)
    int  (*Init)(unsigned int ElementInfoAddress, unsigned int RegionSize,const i_ElementInfoEvent_t* const ElementInfoEvent);
	int (*ToString)(const s_ElementInfo_t* ElementInfo,char* Buffer);
    /* other methods */

end_def_interface(i_ElementInfo_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_ElementInfo_t* const zmElementInfoManager;
extern const s_ElementInfo_t* zmElementInfos;

/*============================ PROTOTYPES ====================================*/

#if defined(__ElementInfo_CLASS_IMPLEMENT__) || defined(__ElementInfo_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void ElementInfo_protected_method_example(c_ElementInfo_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __ElementInfo_CLASS_INHERIT__
#undef __ElementInfo_CLASS_IMPLEMENT__

#endif
/* EOF */