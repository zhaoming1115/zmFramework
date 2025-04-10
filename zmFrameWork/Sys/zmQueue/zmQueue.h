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
文 件 名： zmQueue.h 
开发日期：2025-04-09 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __ZM_QUEUE_H_
#define __ZM_QUEUE_H_

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
   
#if     defined(__Queue_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__Queue_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"

#define	Queue_DataLength_OneFrame			0xffff	//该值代表一帧数据长度，队列元素大小为1时，代表实际需要操作的字节数为65535，若缓存区没这么大，则会按照缓存区大小来操作
#define Queue_UseFullAndEmptyAlarm			1		//使用队列满告警

#define __Queue_ElementSizeUsePower2		1		//0-元素大小使用真实的大小，1-元素大小使用基2的指数
typedef enum 
{
	Queue_OPRST_Base=-16,	//
	Queue_OPRST_Fulled,		//队列满，即队列中帧数量大于等于队列中可容纳的帧数量
	Queue_OPRST_Emptyed,		//队列空，此时缓存也应该为空
	Queue_OPRST_ThisFrameIsPart,		//这个帧只是一部分，该异常在两种情况下返回：1-在请求出队的数量小于一帧的数量时；2-请求入队的数量大于0且刚好是元素大小的整倍数
	Queue_OPRST_NoEnoughBuffer,		//没有足够的缓存，在入队元素数量大于等于队列剩余空间时会触发该异常。
//	Queue_RST_OKAndPart=-2,			//正常，但是是一部分
	Queue_RST_OK=0,					//正常
	//大于0代表数量
}e_QueueOperationResult_t;

typedef enum 
{
	Queue_RegionState_Empty,
	Queue_RegionState_HaveData,
	Queue_RegionState_Full,
}e_QueueRegionState_t;

typedef union
{
	unsigned char Value;
	struct
	{
		unsigned char PartFrameFlag:1;
#if Queue_UseFullAndEmptyAlarm
		unsigned char FullFlag:1;
		unsigned char EmptyFlag:1;
		unsigned char BaoLiu:5;
#else
		unsigned char BaoLiu:7;

#endif
	};
}u_QueueAlarmState_t;

typedef union
{
	unsigned short Value;
	struct
	{
		u_QueueAlarmState_t AlarmState;
		unsigned short UserState;
	};
}u_QueueState_t;

//! \name class Queue_t
//! @{
declare_class(c_Queue_t)
declare_interface(i_Queue_t)

#if __Queue_ElementSizeUsePower2

typedef struct Queue_cfg_t {
    
    //! put your configuration members here
	unsigned char ElementSizeLevel;		//元素大小等级，基2
	unsigned char UserData;		//用户数据
    unsigned short ElementCount;	//元素数量
} s_Queue_cfg_t;

/*添加一个队列配置
__ElementSizeLevel：元素大小等级，基2
__ElementCount：元素数量
*/
#define Append_QueueConfig(__ElementSizeLevel,__ElementCount)		{.ElementSizeLevel=__ElementSizeLevel,.ElementCount=__ElementCount}

#else
typedef struct Queue_cfg_t {   
	unsigned short ElementSize;		//元素大小
    unsigned short ElementCount;	//元素数量
} s_Queue_cfg_t;
#define Append_QueueConfig(__ElementSize,__ElementCount)		{.ElementSize=__ElementSize,.ElementCount=__ElementCount}

#endif


def_class(c_Queue_t,

    public_member(
        //!< place your public members here
		const i_Queue_t* Manager;
    )

    private_member(
        //!< place your private members here
    )
    
    protected_member(
        //!< place your private members here
		const s_Queue_cfg_t Config;
		unsigned short ReadIndex;
		unsigned short WriteIndex;
		unsigned short ElementCount;
		u_QueueState_t State;
    )
)

end_def_class(c_Queue_t) /* do not remove this for forward compatibility  */
//! @}



//! \name interface i_Queue_t
//! @{
def_interface(i_Queue_t)
    c_Queue_t *  (*Create) (const s_Queue_cfg_t *ptCFG);
    void         (*Dispose)(c_Queue_t *ptObj);
/*****************************************************************************
* 函 数 名： Enqueue
* 函数功能： 入队一个或多个元素
* 参    数： Queue: 队列对象 
**			 Data: [输入]：要入队的数据流,若为空，不复制数据
**			 DataLength: 要入队的数据流长度 
* 返    回： 若成功，返回实际入队的数据长度，否则返回异常码，异常码见e_QueueOperationResult_t
* 描    述： 入队的数据流可小于元素大小，此时占用一个元素的空间；
			 也可大于元素大小，此时占用多个元素空间，且向上取整；
			若缓存区没有足够的空间，则返回Queue_OPRST_NoEnoughBuffer

* 开 发 者：赵明 
* 日    期： 2024-03-24

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	int (*Enqueue)(c_Queue_t* Queue,__In char* Data,unsigned int DataLength);
	
/*****************************************************************************
* 函 数 名： Dequeue
* 函数功能： 出队一组数据
* 参    数： Queue: 队列对象 
**			 Data: 数据去向，若为空，则不复制数据 
**			 DataLength: 要出队的数据最大字节数
* 返    回： 若成功，实际出队的数据字节数，否则返回异常码，异常码见e_QueueOperationResult_t
* 描    述： 当DataLength大于等于队列中第一整帧数据长度时，出队一帧数据，并返回实际出队的字节量；
			 当DataLength小于队列中第一整帧数据长度时，出队的数据量为L,L=DataLength-DataLength % ElementSize,并返回Queue_OPRST_DstBufferOverflow；

* 开 发 者：赵明 
* 日    期： 2024-03-24

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	int (*Dequeue)(c_Queue_t* Queue,__Out char* Data,unsigned int DataLength);
	
/*****************************************************************************
* 函 数 名： AskEnqueue
* 函数功能： 申请入队
* 参    数： Queue: 队列对象 
**			 DataBuffer: [输出]：申请到的缓存地址
* 返    回： 若成功，返回申请到的缓存长度，该长度为ElementSize的整数倍
* 描    述： 本函数常用于为接收DMA提供缓存空间，在DMA传输结束后，需要调用Enqueue函数将数据正式入队，这种情况下需要将Enqueue中的Data参数设置为空

* 开 发 者：赵明 
* 日    期： 2024-03-24

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	int (*AskEnqueue)(c_Queue_t* Queue,__Out char** DataBuffer);

/*****************************************************************************
* 函 数 名： int
* 函数功能： 申请出队
* 参    数： Queue: 队列对象
**			 DataBuffer: [输出]：队首的数据缓存地址
* 返    回：若成功，返回本次可出队的数据长度，该长度为一帧数据的实际长度，或者是ElementSize的整数倍
* 描    述：本函数常用于为发送DMA提供数据源区间，在DMA传输结束后，需要调用Dequeue函数将数据正式出队，这种情况下需要将Dequeue中的Data参数设置为空

* 开 发 者：赵明 
* 日    期： 2024-03-24

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	int (*AskDequeue)(c_Queue_t* Queue,__Out const char** DataBuffer);

	int (*ByteCount)(c_Queue_t* Queue);
	int (*RemBufferSize)(c_Queue_t* Queue);
	int (*Clear)(c_Queue_t* Queue);
	u_QueueState_t (*GetState)(c_Queue_t* Queue);
    /* other methods */

end_def_interface(i_Queue_t) /*do not remove this for forward compatibility */
//! @}

#if __Queue_ElementSizeUsePower2
#define Queue_GetElementSize()			(1<<cfg->ElementSizeLevel)
inline static unsigned int Queue_remElementSize(__In s_Queue_cfg_t* Config,unsigned int Size)
{
	return Size & ((1<<Config->ElementSizeLevel)-1);
}

inline static unsigned int Queue_AlignToElementSize(__In s_Queue_cfg_t* Config,unsigned int Size,__Out unsigned int* ElementCount)
{
	*ElementCount=Size>>Config->ElementSizeLevel;
	return Size & ~((1<<Config->ElementSizeLevel)-1);
}

#else
#define Queue_GetElementSize()		cfg->ElementSize
inline static unsigned int Queue_remElementSize(__In s_Queue_cfg_t* Config,unsigned int Size)
{
	return Size % Config->ElementSize;
}

inline static unsigned int Queue_AlignToElementSize(__In s_Queue_cfg_t* Config,unsigned int Size,__Out unsigned int* ElementCount)
{
	div_t  result=div(Size,Config->ElementSize);
	*ElementCount=result.quot
	return Size - result.rem;
}

#endif


/*============================ GLOBAL VARIABLES ==============================*/

/*============================ PROTOTYPES ====================================*/

#if defined(__Queue_CLASS_IMPLEMENT__) || defined(__Queue_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void Queue_protected_method_example(c_Queue_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __Queue_CLASS_INHERIT__
#undef __Queue_CLASS_IMPLEMENT__

#endif
/* EOF */
