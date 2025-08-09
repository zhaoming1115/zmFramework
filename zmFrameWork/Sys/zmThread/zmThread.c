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
/*���ļ�����PLOOC������������ԭ��˵��*/
/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� zmThread.c 
�������ڣ�2025-03-28 
�ļ����ܣ�ʵ���̹߳���������

��    �������������ڹ������ProtoThread��ͨ���������Ķ�ʱ�����������Ϊ2�����ȼ���

�����ӿڣ�HAL_Interface_ThreadTimer.h �ļ��еĽӿ�

�ṩ�ӿڣ�zmThreadManager����
	

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

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
	Thread_State_Alive,		//�����
	Thread_State_Run,			//��������
	Thread_State_WeiHu,		//����ά��
	Thread_State_Dead,		//�ȴ�ɾ��
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

/******************************* ����д���� **************************************/
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

/******************************* ˽�к��� **************************************/
/*
	�������ƣ�Thread_Service
	���������*Thread-��ǰ�߳�ָ��
	�������ܣ��̷߳������
	����˵��������Ҫɾ�������������������ɾ�������ǲ�������
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
				if (_->ThreadState==Thread_State_Dead)		//����߳�Ϊ��Ҫɾ����״̬
				{
	//				PreItem->ThreadState=Thread_State_WeiHu;			//������̹߳������̵߳�״̬��Ϊά��״̬�����Ⲣ����ͻ
	//				this->ThreadState=Thread_State_WeiHu;
					zmListManager->Remove(CurList,CurrentItem);
					memset(_,0,sizeof(*_));		//�߳���ռ���ڴ�������
				}
				PreItem=zmListManager->Next(CurList,PreItem);	//���ӵ���һ���߳�
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
			if (_->ThreadState==Thread_State_Dead)		//����߳�Ϊ��Ҫɾ����״̬
			{
				memset(_,0,sizeof(*_));		//�߳���ռ���ڴ�������
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
//	Sys_PrintfMainInfo("���������%p,����ص�ַ��%p,�����������%d\n",&g_RunParm,_first,g_Config->MaxThreadCount);
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

		if (Thread_CanRun(this) && Thread_SubbTick(this->NextCallTick)>=0)	//�жϵ�ǰʱ���Ƿ�����´ε���ʱ��
		{
//					this->ThreadState=Thread_State_Run;
			ExeRst=this->Porcess();
//					this->ThreadState=Thread_State_Alive;
//					CurrentThread->NextCallTick &=0XFFFFFF;
		}
		CurrentItem=zmListManager->Next(CurList,CurrentItem);	//���ӵ���һ������
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
		if (tmp>=0)	//�����ʱ�䵽���������ټ���Ѱ��
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
		CurrentItem=zmListManager->Next(CurList,CurrentItem);	//���ӵ���һ������
	}
	TimeSpan=Thread_ConvToTimerTickSpan(TimeSpan);
	if(TimeSpan<MinDelay) TimeSpan=MinDelay;
	UH_ThreadTimer_OnInterrupt(TimeSpan);
}

/*
	�����ȼ��߳���ѯ���ȳ��򣬱����ȼ��²�����ʵʱ��ѯ������
	�����߼���ʹ�õ�������ʽ����ѭ������������״ִ̬����Ӧ���߼���
						�������״̬Ϊ����״̬�����жϵ��ȼ���Ƿ����0���ǵĻ������ʱ�䵽���Ժ󣬿�ʼִ������������ִ����Ϻ󣬸����´ε���ʱ��Ϊ���ε���ʱ����ϵ��ȼ����
																		                         �����ж��Ƿ񵥴������ǵĻ�����ִ������	������ִ����Ϻ󣬽�������Ϊɾ��״̬��											
						�������״̬Ϊά��״̬�����жϵ��ȼ���Ƿ����0�Ҽ��ʱ���Ѿ����ˣ���֪ͨά�������������Ҫִ�С�
																		                         �����ж��Ƿ񵥴������ǵĻ�֪ͨά�������������Ҫִ�С�
						��������״̬�ݲ�����
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
		case Thread_State_Alive:	//����״̬������
				if (Thread_GetTickSpan(this->NextCallTick,Thread_GetTick())>=0)	//������ü������0��ʱ�䵽��
				{
//					CurrentThread->ThreadState=Thread_State_Run;
					ExeRst=this->Porcess();
//					CurrentThread->ThreadState=Thread_State_Alive;
					//					CurrentThread->NextCallTick &=0XFFFFFF;
				}
			break;

		case Thread_State_WeiHu:	//ά��״̬������
			if (Thread_GetTickSpan(this->NextCallTick,Thread_GetTick())>=0)
			{
				this->OtherCallFlag=1;		//֪ͨ���ڵ��õĳ��򣬸��߳��в�������
				OtherFlag=true;
			}
			break;

		default:
			break;
		}
		CurrentItem=zmListManager->Next(CurList,CurrentItem);	//���ӵ���һ������
	} 
	if (!OtherFlag)		//���û�в�����ͻ����������һ������Ĵ���ʱ��
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
	�������ƣ�Thread_GetThreadWithFunAndList
	�������ܣ������߳�������ָ�����߳������в����߳�
	���������*Fun(JG_ThreadLX* Thread)-�߳�����
						*ThreadList-�߳�����
	���أ��ҵ����̣߳����û���ҵ����򷵻�NULL
*/
static c_ListItem_t* Thread_GetThreadItemWithFunAndList(f_ThreadProcess Process,c_List_t* ThreadList)
{
	c_ListItem_t* Item=zmListManager->Find(ThreadList,&Process,Thread_FindCHeckFun);
	return Item;
}

/*
	�������ƣ�Thread_GetThreadWithFun
	�������ܣ������߳������������߳������в����߳�
	���������*Fun(JG_ThreadLX* Thread)-�߳�����
	���أ��ҵ����̣߳����û���ҵ����򷵻�NULL
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
	�������ƣ�Thread_AddThread
	�������ܣ����߳���ӵ��߳��б�
	���������*NewThread-Ҫ��ӵ����߳�
						*ThreadList-Ҫ��ӵ����߳��б�
	���أ�0
*/
inline static int Thread_AddThread(c_Thread_t* NewThread,c_List_t* ThreadList)
{
	c_Thread_t* innThread=NewThread;
	zmListManager->Add(ThreadList,&NewThread->use_as__c_ListItem_t);
	return 0;
}

/*
	�������ƣ�Thread_RemoveThread
	�������ܣ����߳���Ϊ��ɾ��״̬
	���������*rmThread-Ҫɾ�����߳�
	���أ�0
*/
static void Thread_RemoveThread(c_Thread_t* Thread)
{
	this_from_implenment(G_CurThread,s_ProtoThread_t,c_Thread_t);
	if(Thread) this=Thread;
	this->ThreadState=Thread_State_Dead;
}


/*
	�������ƣ�Thread_CreateAddThread
	�������ܣ�����������߳�
	���������*Fun(JG_ThreadLX* Thread)-�߳�����
						CallTickSpan-���ü��
						YouXianJi-���ȼ�
	���أ��������̣߳���������Ѿ������̣߳��򷵻���Ӧ���߳�
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
	�������ƣ�Thread_RemoveThreadWithFun
	�������ܣ��������������Ӧ���̣߳���������Ϊ��ɾ��״̬
	���������*Fun(JG_ThreadLX* Thread)-Ҫɾ��������
	���أ�0
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
	�������ƣ�Thread_Sleep
	�������ܣ�ָ�����߳�����һ�ε�ʱ��
	���������*Thread-ָ�����߳�
						Tick-Ҫ���ߵ�ʱ��
	���أ���
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