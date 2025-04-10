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
文 件 名： zmThread.h 
开发日期：2025-03-28 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/


#ifndef __zmThread_H__
#define __zmThread_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "zmThread_Simple.h"
#include "../zmList/zmList.h"

/*============================ MACROS ========================================*/
#define Thread_UseGaoJingDuTick				0    //任务使用高精度时戳

#if Thread_UseGaoJingDuTick
#define Thread_TickSizeUse64bit				1
#define Thread_WaitSignalSleepTime			1000
#else
#define Thread_WaitSignalSleepTime			1

#endif						
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
#if Thread_TickSizeUse64bit
typedef long long threadtick_t;
typedef long long threadtickspan_t;

#else
typedef unsigned int threadtick_t;
typedef int threadtickspan_t;

#endif

typedef int (*f_ThreadProcess)(void);

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__Thread_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__Thread_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class Thread_t
//! @{
declare_class(c_Thread_t)

def_class(c_Thread_t,
	which(implement(s_ProtoThread_t))
    public_member(
        //!< place your public members here
    )

    private_member(
        //!< place your private members here
		threadtick_t NextCallTick;	//线程下次调用的时间
		f_ThreadProcess Porcess;		//线程任务
		unsigned char ThreadID;		//线程ID
		unsigned char ThreadState;	//线程状态
		unsigned char OtherCallFlag;	//高优先级调用标志
		unsigned char ThreadYouXianJi;	//线程优先级
    )
    
    protected_member(
        //!< place your private members here
		implement(c_ListItem_t)
    )
)

end_def_class(c_Thread_t) /* do not remove this for forward compatibility  */
//! @}

typedef enum
{
	Thread_YXJ_CallByLunXun,
	Thread_YXJ_CallByTimer,
	Thread_YXJ_Nums,
}e_ThreadYouXianJi_t;

typedef struct Thread_cfg_t {
    
    //! put your configuration members here
	unsigned char MaxThreadCount;		//最大支持的线程数量，线程服务程序根据这个数量分配线程池大小，数量越大，占用的内存也越大
	unsigned char TimerTickUnitLevel;		//中断定时器时戳单位等级，基2，时戳单位TimerTickUnit= zmSys->GetTick() >> TimerTickUnitLevel
	unsigned char TimerIntMinDelayUnit;	//触发定时器立即中断时定时器需要延时的TimerTickUnit个数
	unsigned char REV;		//保留使用
    
} s_Thread_cfg_t;


//! \name interface i_Thread_t
//! @{
def_interface(i_Thread_t)
    int (*Init)(const s_Thread_cfg_t *ptCFG,unsigned int SysTickPrescalerLevel);
	/*****************************************************************************
	* 函 数 名： CreateAddThread
	* 函数功能： 检查并创建线程
	* 参    数： Fun-线程的任务
				YouXianJi-优先级
				CHeckSame-设置为true时，本函数会先检查是否已经创建了指定任务的线程。若有，则返回那个线程，否则创建新线程
				
	* 描    述：
	
	* 开 发 者：赵明 
	* 日    期： 2022/11/29
	
	* 修改历史：
	** 1. 新创建
	*****************************************************************************/
	c_Thread_t* (*CreateAddThread)(f_ThreadProcess Process,e_ThreadYouXianJi_t YouXianJi,bool CHeckSame);

/*
通过线程函数删除线程
Fun():创建线程时传入的函数，模板如下：
int FunName(JG_ThreadLX* zmThread)
{
/局部变量区

/

	ZT_BEGIN(zmThread);



	ZT_END(zmThread);


}
*/
	/*****************************************************************************
	* 函 数 名： RemoveThreadWithFun
	* 函数功能： 移除指向指定任务的所有线程
	* 参    数： Fun-任务指针
	* 返    回： 无
	* 描    述： 
	
	* 开 发 者：赵明 
	* 日    期： 2022/11/29
	
	* 修改历史：
	** 1. 新创建
	*****************************************************************************/
	void (*RemoveThreadWithProcess)(f_ThreadProcess Process);
/*
运行函数
每调用一次，将会对内部的线程列表轮询一遍，并执行符合轮询条件的相关联的任务。
主程序应该讲此函数置于 WHILE(1){}循环体内
*/
	void (*Run)(void);
	/*****************************************************************************
	* 函 数 名： RemoveThread
	* 函数功能：移除指定的线程
	* 参    数： Thread-线程指针,若为空，代表删除当前线程
	* 返    回： 无
	* 描    述：
	
	* 开 发 者：赵明 
	* 日    期： 2022/11/29
	
	* 修改历史：
	** 1. 新创建
	*****************************************************************************/
	void (*RemoveThread)(c_Thread_t* Thread);

/*****************************************************************************
* 函 数 名： Sleep
* 函数功能： 让指定的线程休眠一段时间
* 参    数： Thread: 要休眠的线程指针 
**			 SleepTime:要休眠的时长，对于调度方式为轮询的线程，单位为ms，其它的线程单位为us
			WithCallTickFlag: true-以判断调用的时戳为基准，false-以当前时戳为基准
* 返    回： 无
* 描    述： 对于调度方式为轮询的线程，执行该函数后，将会从当前时间开始，休眠SleepTime指定的毫秒数；
			对于调度方式为Timer的线程，执行该函数后，会延时SleepTime指定的微秒数后再触发该线程的中断；


* 开 发 者：赵明 
* 日    期： 2024-02-01

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	void (*Sleep)(unsigned int SleepTime,bool WithCallTickFlag);

end_def_interface(i_Thread_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_Thread_t* const zmThreadManager;

#define ZT_Sleep2(__SleepTime,__WithCallTickFlag) 		zmThreadManager->Sleep(__SleepTime,__WithCallTickFlag); \
														ZT_YIELD_SLEEP()
#define ZT_Sleep1(__SleepTime) 							ZT_Sleep2(__SleepTime,false)

#define ZT_Sleep(...)  									SYS_EVAL(ZT_Sleep, __VA_ARGS__)(__VA_ARGS__)

#define ZT_SubSleep(__Thread,__SleepTime) 				zmThreadManager->Sleep(__SleepTime,false); \
														ZT_YIELD_SLEEP(__Thread)

#define ZT_WaitSignal2(__Thread,__Signal) 				while(!(__Signal)){ \
															ZT_SubSleep(__Thread,1);}
#define ZT_WaitSignal1(__Signal) 						while(!(__Signal)){ \
															ZT_Sleep(Thread_WaitSignalSleepTime);}
#define ZT_WaitSignal(...)  							SYS_EVAL(ZT_WaitSignal, __VA_ARGS__)(__VA_ARGS__)

/*============================ PROTOTYPES ====================================*/

#if defined(__Thread_CLASS_IMPLEMENT__) || defined(__Thread_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void Thread_protected_method_example(c_Thread_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __Thread_CLASS_INHERIT__
#undef __Thread_CLASS_IMPLEMENT__

#endif
/* EOF */