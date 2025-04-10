/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： UH_ThreadTimer.c 
开发日期：2025-04-09 
文件功能：

描    述：

依赖接口：

提供接口：
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/
#include "..\Inc\UH_ThreadTimer.h"

#define __ThreadTimer				TIM6
#define __MaxTickValue				0xffff
#define __TickIn1us					1

unsigned int UH_ThreadTimer_Init(unsigned short Prescaler)
{
    LL_TIM_InitTypeDef TIM_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);

    /* USER CODE BEGIN TIM6_Init 1 */

    /* USER CODE END TIM6_Init 1 */
    TIM_InitStruct.Prescaler = Prescaler-1;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 65535;
    LL_TIM_Init(TIM6, &TIM_InitStruct);

    LL_TIM_DisableARRPreload(__ThreadTimer);
    LL_TIM_SetOnePulseMode(__ThreadTimer, LL_TIM_ONEPULSEMODE_SINGLE);
    LL_TIM_EnableIT_UPDATE(__ThreadTimer);
    NVIC_SetPriority(TIM6_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),ZHL_ThreadTimer_IRQPriority, 0));
    NVIC_EnableIRQ(TIM6_IRQn);
	return Prescaler;
}

inline static unsigned int UH_ThreadTimer_usToTick(unsigned int us)
{
    return __TickIn1us*us;
}

/*****************************************************************************
* 函 数 名： UH_ThreadTimer_OnInterrupt
* 函数功能：设置定时器中断
* 参    数： us-延时时间
* 返    回： void
* 描    述：本函数用于设置一个定时器，令其在指定的微秒时间后产生中断，用于调度线程管理器中的高优先级任务

* 开 发 者：赵明
* 日    期： 2022/11/30

* 修改历史：
** 1. 新创建
*****************************************************************************/
inline void UH_ThreadTimer_OnInterrupt(unsigned int TickUnit)
{
    unsigned int TickCount=TickUnit;
    if (TickCount>=__MaxTickValue)
    {
        TickCount=__MaxTickValue;
    }
    else if (TickCount==0)
    {
        TickCount=1;
    }
    __ThreadTimer->ARR=TickCount;
    LL_TIM_EnableCounter(__ThreadTimer);
}

inline void UH_ThreadTimer_OffInterrupt(void)
{
    LL_TIM_DisableCounter(__ThreadTimer);
}

void __ThreadTimer_HANDLER()
{
    LL_TIM_ClearFlag_UPDATE(__ThreadTimer);
    UH_ThreadTimer_Interrupt();
}
