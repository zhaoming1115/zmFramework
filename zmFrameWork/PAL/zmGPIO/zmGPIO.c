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
文 件 名： zmGPIO.c 
开发日期：2025-03-26 
文件功能：实现GPIO管理器类对象

描    述：

依赖接口：HAL_Interface_GPIO.h 文件中的接口

提供接口：
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/


/*============================ INCLUDES ======================================*/
#define __GPIO_CLASS_IMPLEMENT__

#include "zmGPIO.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <cmsis_compiler.h>
#include "HAL_Interface/HAL_Interface_GPIO.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)    assert(__VA_ARGS__)
#endif

#undef this
#define this        (ptThis)

/*============================ TYPES =========================================*/
typedef struct
{
	unsigned char ExITCount;
}s_GPIO_RunParm_t;

typedef struct
{
	gpio_t GPIOIndex;
	f_GPIOInputCHanged CallBack;
}s_GPIOEventKeyValuePair_t;

/*============================ LOCAL VARIABLES ===============================*/
static s_GPIOEventKeyValuePair_t* g_GPIOEventList=NULL;
static s_GPIO_RunParm_t g_RunParm={0};
#define g_ExITCount			g_RunParm.ExITCount
#define g_GPIONone			GPIO_None

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

/**************************** 可重定义的函数 **********************************************/
__WEAK void GPIO_NormalInHandler(gpio_t GPIOx)
{
	(void)GPIOx;
}

/*******************************驱动回调函数 ***************************************************/
void UHC_GPIO_InHandler(gpio_t GPIOx)
{
	s_GPIOEventKeyValuePair_t* GPIOEvent=g_GPIOEventList;
	for(int i=0;i<g_ExITCount;i++,GPIOEvent++)
	{
		if(GPIOEvent->GPIOIndex==GPIOx)
		{
			if(GPIOEvent->CallBack) GPIOEvent->CallBack();
			break;
		}
	}
	GPIO_NormalInHandler(GPIOx);
}

/*============================ IMPLEMENTATION ================================*/

/*! \brief the constructor of the class: GPIO */
static
int GPIO_Init(unsigned char SurportExITCount)
{
	g_ExITCount=SurportExITCount;

	if(g_ExITCount>0)
	{
		if(g_GPIOEventList) free(g_GPIOEventList);
		unsigned int len=sizeof(s_GPIOEventKeyValuePair_t)*g_ExITCount;
		g_GPIOEventList=malloc(len);
		if(g_GPIOEventList==NULL) 
		{
			GPIO_PrintError("GPIO管理器申请内存失败\n");
			return GPIO_Err_ApplyMemoryFail;
		}
		memset(g_GPIOEventList,g_GPIONone,len);
	}
	UH_GPIO_Init();
	return 0;
}

static
 int GPIO_SetExitIT(gpio_t GPIOx,e_Exit_TriggerMode_t TriggerMode,f_GPIOInputCHanged CallBack)
{
	int rst=UH_GPIO_SetExitIT(GPIOx,TriggerMode);
	if(rst<0) return rst;
	if(CallBack)
	{
		int id=-1;
		s_GPIOEventKeyValuePair_t* GPIOEvent=g_GPIOEventList;
		for(int i=0;i<g_ExITCount;i++,GPIOEvent++)
		{
			if(GPIOEvent->GPIOIndex==GPIOx)
			{
				GPIOEvent->CallBack=CallBack;
				return 0;
			}
			else if(GPIOEvent->GPIOIndex==g_GPIONone && id==-1)
			{
				id=i;
			}
		}
		if(id>=0)
		{
			GPIOEvent=g_GPIOEventList+id;
			GPIOEvent->CallBack=CallBack;
			GPIOEvent->GPIOIndex=GPIOx;
		}
	}
	return 0;
}

static
int GPIO_ClearExitIT(gpio_t GPIOx)
{
	int rst=UH_GPIO_ClearExitIT(GPIOx);
	if(rst<0) return rst;
	int id=-1;
	s_GPIOEventKeyValuePair_t* GPIOEvent=g_GPIOEventList;
	for(int i=0;i<g_ExITCount;i++,GPIOEvent++)
	{
		if(GPIOEvent->GPIOIndex==GPIOx)
		{
			GPIOEvent->GPIOIndex=g_GPIONone;
			GPIOEvent->CallBack=NULL;
			break;
		}
	}
	return 0;
}

#define GPIO_SetMode			UH_GPIO_SetMode
#define GPIO_ReadPin			UH_GPIO_ReadPin
#define GPIO_WritePin			UH_GPIO_WritePin

const i_GPIO_t* const zmGPIOManager = &(const i_GPIO_t){
    .Init =             &GPIO_Init,
    .SetMode =          &GPIO_SetMode,
    .ReadPin=			&GPIO_ReadPin,
	.WritePin=			&GPIO_WritePin,
	.SetExitIT=			&GPIO_SetExitIT,
	.ClearExitIT=		&GPIO_ClearExitIT
    /* other methods */
};


/* EOF */