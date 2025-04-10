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
文 件 名： zmPWM.c 
开发日期：2025-03-28 
文件功能：实现PWM管理器类对象

描    述：

依赖接口：HAL_Interface_PWM.h 文件中的接口

提供接口：zmPWMManager对象
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/


/*============================ INCLUDES ======================================*/
#define __PWM_CLASS_IMPLEMENT__

#include "zmPWM.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "../../HAL_Interface/HAL_Interface_PWM.h"

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
int PWM_Init(c_PWM_t *ptObj, const s_PWM_cfg_t *ptCFG)
{
	class_internal(ptObj, ptThis, c_PWM_t);		
	UH_PWM_Init(this->CHannelIndex,ptCFG->PinLv,ptCFG->JingDu);
	return 0;
}

static
void PWM_DeInit(c_PWM_t *ptObj)
{
	class_internal(ptObj, ptThis, c_PWM_t);		
}

static
void PWM_Start(c_PWM_t *ptObj)
{
	class_internal(ptObj, ptThis, c_PWM_t);		
	UH_PWM_Start(this->CHannelIndex);
}

static
void PWM_Stop(c_PWM_t *ptObj)
{
	class_internal(ptObj, ptThis, c_PWM_t);		
	UH_PWM_Stop(this->CHannelIndex);
}

static 
void PWM_SetMaiKuan(c_PWM_t *ptObj,unsigned int WanFenBi)
{
	class_internal(ptObj, ptThis, c_PWM_t);		
	unsigned int tmp=UH_PWM_GetJingDu(this->CHannelIndex)*WanFenBi;
	UH_PWM_SetMaiKuan(this->CHannelIndex,(tmp+10000/2)/10000);
}

static 
void PWM_SetMaiKuanWithTick(c_PWM_t *ptObj,unsigned int PWMTick)
{
	class_internal(ptObj, ptThis, c_PWM_t);		
	UH_PWM_SetMaiKuan(this->CHannelIndex,PWMTick);
}

static 
unsigned int PWM_GetMaiKuan(c_PWM_t *ptObj)
{
	class_internal(ptObj, ptThis, c_PWM_t);		
	unsigned int tmp=UH_PWM_GetMaiKuan(this->CHannelIndex);
	unsigned int tmp1=UH_PWM_GetJingDu(this->CHannelIndex);
	return (tmp*10000+(tmp1>>1))/tmp1;
}

static 
unsigned int PWM_GetMaiKuanWithTick(c_PWM_t *ptObj)
{
	class_internal(ptObj, ptThis, c_PWM_t);		
	int tmp=UH_PWM_GetMaiKuan(this->CHannelIndex);
	return tmp;
}

const i_PWM_t* const zmPWMManager = &(const i_PWM_t){
    .Init =             &PWM_Init,
    .DeInit =           &PWM_DeInit,
	.Start =			&PWM_Start,
	.Stop =				&PWM_Stop,
	.SetMaiKuan =		&PWM_SetMaiKuan,
	.GetMaiKuan =		&PWM_GetMaiKuan,
	.SetMaiKuanWithTick=&PWM_SetMaiKuanWithTick,
	.GetMaiKuanWithTick=&PWM_GetMaiKuanWithTick
    
    /* other methods */
};


/* EOF */