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
版权所有：赵明
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： zmSys.c 
开发日期：2025-03-28 
文件功能：实现系统核心功能

描    述：

依赖接口：

提供接口：zmSys对象
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/


/*============================ INCLUDES ======================================*/
#define __zmSys_CLASS_IMPLEMENT__

#include "HAL_Interface/HAL_Interface_Sys.h"
#include "zmSys.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/*============================ MACROS ========================================*/
#define Sys_TickIntPerMillSecond	
#define Sys_RunFlag					"时戳"

/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)    assert(__VA_ARGS__)
#endif

#undef this
#define this        (ptThis)

#define Sys_uTickUnit()											g_RunParm->uTickUnit
		
#define Sys_GetRemTick()										(SysTick->LOAD-SysTick->VAL)
											
#define Sys_GetTickValue_Safe(CounterValue,remTickValue)		do{remTickValue=Sys_GetRemTick(); \
																	CounterValue=g_RunParm->mSecondCounter; \
																}while(remTickValue>Sys_GetRemTick())

/*============================ TYPES =========================================*/
typedef struct 
{
	volatile long long mSecondCounter;//总毫秒计数器

	unsigned short mTickSpan_FeedDog;//喂狗间隔毫秒数
	unsigned short GetTickUseTicks;
	unsigned short uTickUnit;
#ifndef Sys_TickIntPerMillSecond
	unsigned short mTickIntUnit;		//时戳最大值中断对应值，基本计数器累加此值
	char RunFlag[4];
#else
	char RunFlag[2];

#endif	

}s_RunParm_t;


/*============================ LOCAL VARIABLES ===============================*/
static s_RunParm_t __g_RunParm __NoInitRAM;																
static s_RunParm_t* g_RunParm=&__g_RunParm;

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

/*============================ 私有函数 =========================================*/
static int Sys_InitTickTimer(unsigned int ClockDivLevel,unsigned int IntMillSecond)
{
	unsigned int Clock=SystemCoreClock>>ClockDivLevel;
	
	unsigned int msUnit=Clock/1000;
#ifdef Sys_TickIntPerMillSecond
	IntMillSecond=1;
#else
	while(msUnit*IntMillSecond>=0x1000000)
	{
		IntMillSecond>>=1;
	}
#endif
	SysTick->LOAD=msUnit*IntMillSecond-1;
	SysTick->VAL=SysTick->LOAD;
	if(ClockDivLevel)
	{
		SysTick->CTRL=SysTick_CTRL_ENABLE_Msk|SysTick_CTRL_TICKINT_Msk;
	}
	else
	{
		SysTick->CTRL=SysTick_CTRL_ENABLE_Msk|SysTick_CTRL_TICKINT_Msk|SysTick_CTRL_CLKSOURCE_Msk;
	}
	NVIC_SetPriority(SysTick_IRQn,0);
	return IntMillSecond;
}

/*============================ IMPLEMENTATION ================================*/

void SysTick_Handler(void)		//滴答定时器中断
{
#ifdef Sys_TickIntPerMillSecond
	g_RunParm->mSecondCounter++;
#else
	g_RunParm->mSecondCounter+=g_RunParm->mTickIntUnit;
#endif
}

/*! \brief the constructor of the class: zmSys */

static
tick_t Sys_GetTick(void)
{
	tick_t Counter;
	unsigned int remTick;
	Sys_GetTickValue_Safe(Counter,remTick);
	
	unsigned int TicksPerInt=SysTick->LOAD+1;
	return Counter*TicksPerInt+remTick;
}

static
int Sys_Init(const s_zmSys_cfg_t* const MCUConfig,unsigned char ClockDivLevel)
{
	int rst=0;
	if(memcmp(g_RunParm->RunFlag,Sys_RunFlag,sizeof(g_RunParm->RunFlag)))
	{
		memset(g_RunParm,0,sizeof(*g_RunParm));
		memcpy(g_RunParm->RunFlag,Sys_RunFlag,sizeof(g_RunParm->RunFlag));
	}
#ifdef Sys_TickIntPerMillSecond
	(void)Sys_InitTickTimer(ClockDivLevel,MCUConfig->IntMillSeconds);
	g_RunParm->uTickUnit=(SysTick->LOAD+1)/1000;

#else
	g_RunParm->mTickIntUnit= Sys_InitTickTimer(Clock,MCUConfig->IntMillSeconds);
	g_RunParm->uTickUnit=(SysTick->LOAD+1)/(g_RunParm->mTickIntUnit*1000);

#endif
	tick_t StartTick=Sys_GetTick();
	volatile tick_t EndTick;
	for(int i=0;i<16;i++)
	{
		EndTick=Sys_GetTick();
	}
	g_RunParm->GetTickUseTicks=(EndTick-StartTick)>>4;

	if(MCUConfig->mTickSpan_FeedDog>0)
	{
		g_RunParm->mTickSpan_FeedDog=UH_Sys_StartKMGou(MCUConfig->mTickSpan_FeedDog);
	}
	else
	{
		g_RunParm->mTickSpan_FeedDog=0xffff;
	}
	return rst;
}

static
void Sys_ReStart(void)
{
	__disable_irq();
	NVIC_SystemReset();//
}

static
void Sys_EntryLowPower(unsigned int Level)
{
	UH_Sys_EntryLowPower(Level);
}

static
tick_t Sys_GetmTick(void)
{
	volatile typeof(g_RunParm->mSecondCounter) Counter=g_RunParm->mSecondCounter;
#if __CORTEX_M<3
	if(Counter>g_RunParm->mSecondCounter)
	{
		Counter=g_RunParm->mSecondCounter;
	}
#else
	return Counter;

#endif
}

static
tick_t Sys_GetmTickSpan(tick_t starttick)
{
	return Sys_GetmTick()-starttick;	
}


static
void Sys_FeedDog(void)
{
	UH_Sys_FeedDog();
}

static void Sys_mDelay_Inner(unsigned int MillSecond)
{
	tick_t beginTick=Sys_GetmTick();
	while(Sys_GetmTickSpan(beginTick)<MillSecond);
}

static
void Sys_mDelay(unsigned int MillSecond)	
{
	unsigned int MaxTickPerWait=g_RunParm->mTickSpan_FeedDog>>1;
	while(MillSecond>MaxTickPerWait)
	{
		Sys_mDelay_Inner(MaxTickPerWait);
		Sys_FeedDog();
		MillSecond-=MaxTickPerWait;
	}
	Sys_mDelay_Inner(MillSecond);
}

static
void Sys_uDelay(unsigned int MicroSecond)
{
	unsigned int Ticks=MicroSecond*g_RunParm->uTickUnit;
	volatile tick_t starttick=Sys_GetTick(); 
	if(Ticks<=g_RunParm->GetTickUseTicks) return;	
	tick_t endtick=starttick+Ticks;
	while(Sys_GetTick()<endtick);
}


static
e_Sys_StartFrom_t Sys_GetStartFrom(void)
{
	return UH_Sys_GetStartFrom();
}

static
int Sys_GetUID(__Out unsigned char* UIDData)
{
	return UH_Sys_GetUID(UIDData);
}

static
bool Sys_CHeckUID(__In unsigned char* UIDData)
{
	unsigned char tmpbuf[32];
	int len=Sys_GetUID(tmpbuf);
	return !memcmp(tmpbuf,UIDData,len);
}


const i_zmSys_t* const zmSys = &(const i_zmSys_t){
    .Init =             &Sys_Init,
    .EntryLowPower =    &Sys_EntryLowPower,
    .ReStart=			&Sys_ReStart,
	.mDelay=			&Sys_mDelay,
	.GetStartFrom=		&Sys_GetStartFrom,
	.GetUID=			&Sys_GetUID,
	.CHeckUID=			&Sys_CHeckUID,
	.uDelay=			&Sys_uDelay,
	.GetmTick=			&Sys_GetmTick,
	.GetmTickSpan=		&Sys_GetmTickSpan,
	.FeedDog=			&Sys_FeedDog,
	.GetTick=			&Sys_GetTick
    /* other methods */
};


/* EOF */