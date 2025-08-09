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
文 件 名： zmThread.c 
开发日期：2025-03-28 
文件功能：实现线程管理器对象

描    述：本对象用于管理调度ProtoThread，通过引入额外的定时器，将任务分为2个优先级。

依赖接口：HAL_Interface_ThreadTimer.h 文件中的接口

提供接口：zmThreadManager对象
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/


/*============================ INCLUDES ======================================*/
#define __List_CLASS_IMPLEMENT__
#define __Thread_CLASS_IMPLEMENT__

#include "zmThread.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "../zmSys/zmSys.h"

#ifdef Thread_UseTimer
#include "../../HAL_Interface/HAL_Interface_ThreadTimer.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)    assert(__VA_ARGS__)
#endif

#undef this
#define this        (ptThis)

/*============================ TYPES =========================================*/
typedef enum
{
	Thread_State_Alive,		//激活的
	Thread_State_Run,			//正在运行
	Thread_State_WeiHu,		//正在维护
	Thread_State_Dead,		//等待删除
}e_ThreadState_t;

#define Thread_CanRun(__Thread)		((__Thread)->ThreadState==Thread_State_Alive || (__Thread)->ThreadState==Thread_State_Run)

typedef struct 
{
	c_List_t FirstThread[Thread_YXJ_Nums];
	c_Thread_t* Threads;
#if !Thread_UseGaoJingDuTick
	unsigned int TimerTicksPerms;
#endif
}s_RunParm_t;

/*============================ LOCAL VARIABLES ===============================*/
static const s_Thread_cfg_t* g_Config=NULL;
static s_RunParm_t g_RunParm;

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
s_ProtoThread_t* G_CurThread=NULL;

/******************************* 可重写函数 **************************************/
#if Thread_UseGaoJingDuTick
__WEAK threadtick_t Thread_GetTick(void)
{
	threadtick_t tick=zmSys->GetTick();
	return tick;
}

__WEAK threadtickspan_t Thread_ConvToTimerTickSpan(threadtickspan_t tickspan)
{
	tickspan=(tickspan+(1<<g_Config->TimerTickUnitLevel)-1)>>g_Config->TimerTickUnitLevel;
	if(tickspan>0x7fffffff) tickspan=0x7fffffff;
	return tickspan;
}

static void Thread_InitConvParm(void)
{
}

#else
__WEAK threadtick_t Thread_GetTick(void)
{
	threadtick_t tick=zmSys->GetmTick();
	return tick;
}

__WEAK threadtickspan_t Thread_ConvToTimerTickSpan(threadtickspan_t tickspan)
{
	long long tspan=(long long)tickspan*g_RunParm.TimerTicksPerms;
	return (tspan>0x7fffffff)?0x7fffffff:tspan;
}

static void Thread_InitConvParm(void)
{
	zmSys->mDelay(10);
	tick_t tick1=zmSys->GetTick();
	tick_t mtick1=zmSys->GetmTick();
	unsigned int tmp=tick1/mtick1;
	g_RunParm.TimerTicksPerms= tmp>>g_Config->TimerTickUnitLevel;	
}
#endif

/******************************* 私有函数 **************************************/
/*
	函数名称：Thread_Service
	输入参数：*Thread-当前线程指针
	函数功能：线程服务程序
	函数说明：将需要删除的任务从任务链表中删除，考虑并发处理
*/

static int Thread_Service(void)
{
	c_ListItem_t*	CurrentItem,*PreItem;

	for (int i=0;i<Thread_YXJ_Nums;i++)
	{
		c_List_t* CurList=&g_RunParm.FirstThread[i];
		PreItem=zmListManager->First(CurList);
		
		if(PreItem==NULL) continue;
		CurrentItem=zmListManager->Next(CurList,PreItem);
		while(CurrentItem)
		{
			this_from_implenment(CurrentItem,c_ListItem_t,c_Thread_t)
			{
				if (_->ThreadState==Thread_State_Dead)		//如果线程为需要删除的状态
				{
	//				PreItem->ThreadState=Thread_State_WeiHu;			//将这个线程关联的线程的状态置为维护状态，避免并发冲突
	//				this->ThreadState=Thread_State_WeiHu;
					zmListManager->Remove(CurList,CurrentItem);
					memset(_,0,sizeof(*_));		//线程所占的内存区清零
				}
				PreItem=zmListManager->Next(CurList,PreItem);	//链接到下一个线程
				if(PreItem)
				{
					CurrentItem=zmListManager->Next(CurList,PreItem);
				}
				else
				{
					CurrentItem=NULL;
				}
			}
		}
		CurrentItem=zmListManager->First(CurList);
		father_from_implenment(CurrentItem,c_ListItem_t,firstthread,c_Thread_t)
		{
			if (_->ThreadState==Thread_State_Dead)		//如果线程为需要删除的状态
			{
				memset(_,0,sizeof(*_));		//线程所占的内存区清零
				zmListManager->Remove(CurList,CurrentItem);
			}
		}
	}
	return ZT_ENDED;
}

inline static c_Thread_t* Thread_CreateFirstThread(void)
{
	unsigned int size=sizeof(c_Thread_t)*g_Config->MaxThreadCount;
	g_RunParm.Threads=(c_Thread_t*)Sys_Malloc(size);
	c_Thread_t* _first=g_RunParm.Threads;
	memset(_first,0,size);
	_first->NextCallTick=Thread_GetTick();
	_first->Porcess=Thread_Service;
#ifdef Thread_UseTimer
	_first->ThreadID=1;
	_first->ThreadYouXianJi=Thread_YXJ_CallByLunXun;
	_first->ThreadState=Thread_State_Alive;
#endif
	ZT_INIT((s_ProtoThread_t*)_first);
//	Sys_PrintfMainInfo("任务参数：%p,任务池地址：%p,任务池容量：%d\n",&g_RunParm,_first,g_Config->MaxThreadCount);
	return _first;
}

inline static threadtickspan_t Thread_GetTickSpan(threadtick_t CallTick,threadtick_t Tick)
{
	threadtickspan_t tmp=(threadtickspan_t)(Tick-CallTick);
	return tmp;
}

static threadtickspan_t Thread_SubbTick(threadtick_t Tick)
{
	return Thread_GetTickSpan(Tick,Thread_GetTick());
}

/*============================ IMPLEMENTATION ================================*/

/*! \brief the constructor of the class: Thread */
static
int Thread_init(const s_Thread_cfg_t *ptCFG,unsigned int SysTickPrescalerLevel)
{
    /* initialise "this" (i.e. ptThis) to access class members */
	memset(&g_RunParm,0,sizeof(g_RunParm));
	g_Config=ptCFG;
	c_Thread_t* first=Thread_CreateFirstThread();
	zmListManager->Add(&g_RunParm.FirstThread[0],&first->use_as__c_ListItem_t);
#ifdef Thread_UseTimer
	UH_ThreadTimer_Init(1<<(g_Config->TimerTickUnitLevel+SysTickPrescalerLevel));
#endif
	Thread_InitConvParm();
	return 0;
}

static void Thread_Run(void)
{
	c_List_t* CurList=&g_RunParm.FirstThread[0];
	c_ListItem_t* CurrentItem=zmListManager->First(CurList);
	int ExeRst;

	while (CurrentItem)
	{
		this_from_implenment(CurrentItem,c_ListItem_t,c_Thread_t);
		G_CurThread=&this->use_as__s_ProtoThread_t;

		if (Thread_CanRun(this) && Thread_SubbTick(this->NextCallTick)>=0)	//判断当前时间是否大于下次调用时间
		{
//					this->ThreadState=Thread_State_Run;
			ExeRst=this->Porcess();
//					this->ThreadState=Thread_State_Alive;
//					CurrentThread->NextCallTick &=0XFFFFFF;
		}
		CurrentItem=zmListManager->Next(CurList,CurrentItem);	//链接到下一个任务
	} 
}

#ifdef Thread_UseTimer
static void Thread_OnTimerThread(int MinDelay)
{
	c_List_t* CurList=&g_RunParm.FirstThread[Thread_YXJ_CallByTimer];
	c_ListItem_t* CurrentItem=zmListManager->First(CurList);
	if (CurrentItem==NULL)
	{
		UH_ThreadTimer_OffInterrupt();
		return;
	}
	threadtickspan_t TimeSpan=~(((threadtickspan_t)1)<<(sizeof(threadtickspan_t)*8-1));	
	threadtickspan_t tmp;
	threadtick_t Tick=Thread_GetTick();
	
	while(CurrentItem)
	{
		this_from_implenment(CurrentItem,c_ListItem_t,c_Thread_t);
		tmp=Thread_GetTickSpan(this->NextCallTick,Tick);
		if (tmp>=0)	//如果有时间到的任务，则不再继续寻找
		{
			break;
		}
		else 
		{
			tmp=-tmp;
			if (tmp<=TimeSpan)
			{
				TimeSpan=tmp;
			}
		}
		CurrentItem=zmListManager->Next(CurList,CurrentItem);	//链接到下一个任务
	}
	TimeSpan=Thread_ConvToTimerTickSpan(TimeSpan);
	if(TimeSpan<MinDelay) TimeSpan=MinDelay;
	UH_ThreadTimer_OnInterrupt(TimeSpan);
}

/*
	高优先级线程轮询调度程序，本优先级下不能有实时轮询的任务
	调度逻辑：使用单向链表方式进行循环，按照任务状态执行相应的逻辑：
						如果任务状态为激活状态，则判断调度间隔是否大于0，是的话待间隔时间到了以后，开始执行任务，至任务执行完毕后，更新下次调度时间为本次调度时间加上调度间隔。
																		                         否则判断是否单次任务，是的话立即执行任务，	至任务执行完毕后，将任务置为删除状态。											
						如果任务状态为维护状态，则判断调度间隔是否大于0且间隔时间已经到了，则通知维护程序该任务需要执行。
																		                         否则判断是否单次任务，是的话通知维护程序该任务需要执行。
						其它任务状态暂不处理。
*/

void UH_ThreadTimer_Interrupt(void)
{
	c_List_t* CurList=&g_RunParm.FirstThread[Thread_YXJ_CallByTimer];
	c_ListItem_t* CurrentItem=zmListManager->First(CurList);
	bool OtherFlag=false;
	int ExeRst;
	s_ProtoThread_t* LowThread=G_CurThread;

	while (CurrentItem!=NULL)
	{
		this_from_implenment(CurrentItem,c_ListItem_t,c_Thread_t);
		G_CurThread=&this->use_as__s_ProtoThread_t;
		switch(this->ThreadState)
		{
		case Thread_State_Alive:	//激活状态的任务
				if (Thread_GetTickSpan(this->NextCallTick,Thread_GetTick())>=0)	//如果调用间隔大于0且时间到了
				{
//					CurrentThread->ThreadState=Thread_State_Run;
					ExeRst=this->Porcess();
//					CurrentThread->ThreadState=Thread_State_Alive;
					//					CurrentThread->NextCallTick &=0XFFFFFF;
				}
			break;

		case Thread_State_WeiHu:	//维护状态的任务
			if (Thread_GetTickSpan(this->NextCallTick,Thread_GetTick())>=0)
			{
				this->OtherCallFlag=1;		//通知正在调用的程序，该线程有并发请求
				OtherFlag=true;
			}
			break;

		default:
			break;
		}
		CurrentItem=zmListManager->Next(CurList,CurrentItem);	//链接到下一个任务
	} 
	if (!OtherFlag)		//如果没有并发冲突，则设置下一个任务的触发时间
	{
		Thread_OnTimerThread(g_Config->TimerIntMinDelayUnit);	
	}
	G_CurThread=LowThread;
}

#endif

static bool Thread_FindCHeckFun(c_ListItem_t* Item,const void* FindObject)
{
	this_from_implenment(Item,c_ListItem_t,c_Thread_t);
	typeof(this->Porcess) value=*(typeof(this->Porcess)*)FindObject;
	return this->Porcess==value && this->ThreadState!=Thread_State_Dead;
}
/*
	函数名称：Thread_GetThreadWithFunAndList
	函数功能：根据线程任务在指定的线程链表中查找线程
	输入参数：*Fun(JG_ThreadLX* Thread)-线程任务
						*ThreadList-线程链表
	返回：找到的线程，如果没有找到，则返回NULL
*/
static c_ListItem_t* Thread_GetThreadItemWithFunAndList(f_ThreadProcess Process,c_List_t* ThreadList)
{
	c_ListItem_t* Item=zmListManager->Find(ThreadList,&Process,Thread_FindCHeckFun);
	return Item;
}

/*
	函数名称：Thread_GetThreadWithFun
	函数功能：根据线程任务在所有线程链表中查找线程
	输入参数：*Fun(JG_ThreadLX* Thread)-线程任务
	返回：找到的线程，如果没有找到，则返回NULL
*/
static c_ListItem_t* Thread_GetThreadItemWithFun(f_ThreadProcess Process)
{
	c_ListItem_t* ThreadItem=NULL;
	for (int i=0;i<Thread_YXJ_Nums;i++)
	{
		ThreadItem=Thread_GetThreadItemWithFunAndList(Process,&g_RunParm.FirstThread[i]);
		if (ThreadItem)
		{
			break;
		}
	}
	return ThreadItem;
}

inline static c_Thread_t* Thread_CreateThread(void)
{
	c_Thread_t* Thread=g_RunParm.Threads;

	for (int i=1;i<g_Config->MaxThreadCount;i++)
	{
		if (Thread->Porcess==NULL)
		{
			Thread->ThreadID=i;
			return Thread;
		}
		Thread++;
	}
	return NULL;
}

/*
	函数名称：Thread_AddThread
	函数功能：将线程添加到线程列表
	输入参数：*NewThread-要添加的新线程
						*ThreadList-要添加到的线程列表
	返回：0
*/
inline static int Thread_AddThread(c_Thread_t* NewThread,c_List_t* ThreadList)
{
	c_Thread_t* innThread=NewThread;
	zmListManager->Add(ThreadList,&NewThread->use_as__c_ListItem_t);
	return 0;
}

/*
	函数名称：Thread_RemoveThread
	函数功能：将线程置为待删除状态
	输入参数：*rmThread-要删除的线程
	返回：0
*/
static void Thread_RemoveThread(c_Thread_t* Thread)
{
	this_from_implenment(G_CurThread,s_ProtoThread_t,c_Thread_t);
	if(Thread) this=Thread;
	this->ThreadState=Thread_State_Dead;
}


/*
	函数名称：Thread_CreateAddThread
	函数功能：创建并添加线程
	输入参数：*Fun(JG_ThreadLX* Thread)-线程任务
						CallTickSpan-调用间隔
						YouXianJi-优先级
	返回：创建的线程，如果任务已经创建线程，则返回相应的线程
*/
static c_Thread_t* Thread_CreateAddThread(f_ThreadProcess Process,e_ThreadYouXianJi_t YouXianJi,bool CHeckSame)
{
	if(CHeckSame)
	{
		c_ListItem_t* ThreadItem=Thread_GetThreadItemWithFun(Process);
		if(ThreadItem)
		{
			this_from_implenment(ThreadItem,c_ListItem_t,c_Thread_t);
			if (this->ThreadYouXianJi==YouXianJi)
			{
				return this;
			}
			else
			{
				Thread_RemoveThread(this);
			}
		}
	}
	c_Thread_t* NewThread=Thread_CreateThread();
	if(NewThread)
	{
		ZT_INIT(&NewThread->use_as__s_ProtoThread_t);
		NewThread->Porcess=Process;
		NewThread->ThreadState=Thread_State_Alive;
		NewThread->ThreadYouXianJi=YouXianJi;
		NewThread->NextCallTick=Thread_GetTick();
		Thread_AddThread(NewThread,&g_RunParm.FirstThread[YouXianJi]);

#ifdef Thread_UseTimer
		if (YouXianJi>0 )
		{
			Thread_OnTimerThread(g_Config->TimerIntMinDelayUnit);
		}
#endif
	}
	return NewThread;
}

/*
	函数名称：Thread_RemoveThreadWithFun
	函数功能：根据任务查找相应的线程，并将其置为待删除状态
	输入参数：*Fun(JG_ThreadLX* Thread)-要删除的任务
	返回：0
*/
static void Thread_RemoveThreadWithProcess(f_ThreadProcess Process)
{
	c_ListItem_t* ThreadItem=NULL;
	do
	{
		ThreadItem=Thread_GetThreadItemWithFun(Process);
		if(ThreadItem)
		{
			this_from_implenment(ThreadItem,c_ListItem_t,c_Thread_t);
			Thread_RemoveThread(this);
//			ThreadItem=ThreadItem->Next;
		}
	}while(ThreadItem);
}

/*
	函数名称：Thread_Sleep
	函数功能：指定的线程休眠一段的时间
	输入参数：*Thread-指定的线程
						Tick-要休眠的时间
	返回：无
*/
inline static void Thread_Sleep(unsigned int Tick,bool WithCallTickFlag)
{
	this_from_implenment(G_CurThread,s_ProtoThread_t,c_Thread_t);
	if(WithCallTickFlag)
	{
		this->NextCallTick+=Tick;
	}
	else
	{
		this->NextCallTick=Thread_GetTick()+Tick;
	}
}

const i_Thread_t* const zmThreadManager = &(const i_Thread_t){
    .Init =             &Thread_init,
    .Sleep =            &Thread_Sleep,
	.CreateAddThread=	&Thread_CreateAddThread,
	.RemoveThreadWithProcess=&Thread_RemoveThreadWithProcess,
	.Run=				&Thread_Run,
	.RemoveThread=	&Thread_RemoveThread,
    /* other methods */
};


/* EOF */