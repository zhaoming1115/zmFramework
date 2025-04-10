/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： UH_Sys.c 
开发日期：2024-03-30 
文件功能：

描    述：

依赖接口：

提供接口：
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

#include "..\HAL_Config.h"
#include "../Inc/UH_Sys.h"
#include "stm32f1xx_ll_iwdg.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_pwr.h"

#define UH_MCU_KMGouIsOff()	(LL_IWDG_IsActiveFlag_PVU(IWDG)==0&&LL_IWDG_GetPrescaler(IWDG)==0)

unsigned short UH_Sys_StartKMGou(unsigned short mSecond)
{
#define IWDG_CLKPLV			40000
#define __IWDG_Reload		(int)4000
#define __IWDG_MinTime		(int)(1000*__IWDG_Reload/IWDG_CLKPLV)

#ifdef IWDG
	int tmp=mSecond/__IWDG_MinTime;
	int i=256;
	int j;
	for (j=6;j>0;j--)
	{
		if (tmp>i)
		{
			break;
		}
		i>>=1;
	}

	if (tmp>=(i+(i>>1)))
	{
		j++;
	}
	if(UH_MCU_KMGouIsOff())
	{
		LL_IWDG_Enable(IWDG);
	}
	else
	{
		LL_IWDG_ReloadCounter(IWDG);
	}
	while(LL_IWDG_IsActiveFlag_PVU(IWDG)){};	//等待更新完成标志为0
	LL_IWDG_EnableWriteAccess(IWDG);
	IWDG->PR=j;
	LL_IWDG_SetReloadCounter(IWDG, __IWDG_Reload);
	while (LL_IWDG_IsReady(IWDG) != 1)
	{
	}
	LL_IWDG_ReloadCounter(IWDG);
	return __IWDG_MinTime<<j;
#else
	return 0xffff;
#endif
}

void UH_Sys_EntryLowPower(unsigned int Level)
{
	LL_PWR_EnableWakeUpPin(LL_PWR_WAKEUP_PIN1);
	SET_BIT(PWR->CR, PWR_CR_PDDS);
	SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
	__WFI();
}

void UH_Sys_FeedDog(void)
{
#ifdef IWDG
	if (LL_IWDG_IsActiveFlag_PVU(IWDG)==0&&LL_IWDG_GetPrescaler(IWDG)==0)
	{
		return;
	}
	LL_IWDG_ReloadCounter(IWDG);
#endif
}

e_Sys_StartFrom_t UH_Sys_GetStartFrom(void)
{
	e_Sys_StartFrom_t tmp;
	if (LL_RCC_IsActiveFlag_PORRST())
	{
		tmp= Sys_StartFrom_PowerOn;
	}
	else if (LL_RCC_IsActiveFlag_IWDGRST())
	{
		tmp=  Sys_StartFrom_WDG;
	}
	else if (LL_RCC_IsActiveFlag_PINRST())
	{
		tmp=  Sys_StartFrom_HWRst;
	}
	else if (LL_RCC_IsActiveFlag_SFTRST())
	{
		tmp=  Sys_StartFrom_SWRst;
	}
	else if (LL_PWR_IsActiveFlag_WU())
	{
		LL_PWR_ClearFlag_WU();
		tmp=Sys_StartFrom_WakeUpFromStandby;
	}
	else
	{
		tmp=Sys_StartFrom_Other;
	}
	LL_RCC_ClearResetFlags();
	return tmp;
}

#define __UIDBASE			UID_BASE
int UH_Sys_GetUID(__Out unsigned char* UIDData)
{
	memcpy(UIDData,(const char*)__UIDBASE,12);
	return 12;
}

