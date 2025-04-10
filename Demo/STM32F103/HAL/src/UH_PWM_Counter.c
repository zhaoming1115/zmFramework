/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： UH_PWM_Counter.c 
开发日期：2025-04-09 
文件功能：

描    述：

依赖接口：

提供接口：
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/
#include "..\Inc/UH_PWM_Counter.h"

#define	PWMCountPerTIM							(int)4		//每个定时器下面的PWM通道数量，取最大值
#define IS_PWM_CHANNEL(CHannelx)				CHannelx<PWMCountPerTIM
#define TIM_CCER_CCE_BITSPAN					(int)4
#define TIM_CCER_CCE(channelindex)				(TIM_CCER_CC1E<<(channelindex*TIM_CCER_CCE_BITSPAN))
#define TIM_CCER_CCE_ALL						((1<<0*TIM_CCER_CCE_BITSPAN)|(1<<1*TIM_CCER_CCE_BITSPAN)|(1<<2*TIM_CCER_CCE_BITSPAN)|(1<<3*TIM_CCER_CCE_BITSPAN))
#define __PWM_MAXPinLv							(int)1000000

__weak void MX_TIM1_Init(void);
__weak void MX_TIM2_Init(void);
__weak void MX_TIM3_Init(void);
__weak void MX_TIM4_Init(void);

__weak void MX_TIM1_Init(void)
{
}

__weak void MX_TIM2_Init(void)
{
}
__weak void MX_TIM3_Init(void)
{
}

__weak void MX_TIM4_Init(void)
{
}

#define _InitFunctionTable	MX_TIM1_Init,MX_TIM2_Init,MX_TIM3_Init,MX_TIM4_Init
static TIM_TypeDef* const g_PWMTIMTalbe[]={TIM1,TIM2,TIM3,TIM4};

static void UH_PWM_XiuGaiJiCunQi(int* RBase,const int Offset,const int Value);

inline static TIM_TypeDef* UH_PWM_GetTIM(unsigned int channelindex,unsigned int* TimCH)
{
	*TimCH=channelindex % PWMCountPerTIM;
	return g_PWMTIMTalbe[channelindex/PWMCountPerTIM];
}

static void UH_PWM_InitTIM(TIM_TypeDef* TIMx)
{
	void (*InitFunTable[])(void)={_InitFunctionTable};
	for(int i=0;i<sizeof(g_PWMTIMTalbe)/sizeof(g_PWMTIMTalbe[0]);i++)
	{
		if(TIMx==g_PWMTIMTalbe[i])
		{
			InitFunTable[i]();
		}
	}
}

void UH_PWM_Init(unsigned int channelindex,unsigned int pinlv,unsigned int jingdu)
{
	unsigned int _channelindex;
	TIM_TypeDef * PWM_TIM=UH_PWM_GetTIM(channelindex,&_channelindex);
	int _arr;
	if (LL_TIM_IsEnabledCounter(PWM_TIM)==0&&pinlv>0&&jingdu>0)
	{
		CLEAR_BIT(PWM_TIM->CCER, TIM_CCER_CCE(_channelindex));
//		assert_param(IS_PWM_CHANNEL(_channelindex));
		UH_PWM_InitTIM(PWM_TIM);
		LL_RCC_ClocksTypeDef rcc;
		int _pr;
		LL_RCC_GetSystemClocksFreq(&rcc);
		if (rcc.HCLK_Frequency>rcc.PCLK1_Frequency)
		{
			rcc.PCLK1_Frequency<<=1;
		}
		if(pinlv<=0) pinlv=__PWM_MAXPinLv;
		_pr=rcc.PCLK1_Frequency/jingdu/pinlv;
		_arr=jingdu;

		LL_TIM_SetAutoReload(PWM_TIM,_arr);	
		LL_TIM_SetPrescaler(PWM_TIM,_pr);

		LL_TIM_EnableARRPreload(PWM_TIM);
		LL_TIM_EnableCounter(PWM_TIM);
	}
	else
	{
		_arr=LL_TIM_GetAutoReload(PWM_TIM);
	}
	UH_PWM_XiuGaiJiCunQi((int*)&PWM_TIM->CCR1,_channelindex,_arr>>1);
}

void UH_PWM_Start(unsigned int channelindex)
{
	unsigned int _channelindex;
	TIM_TypeDef * PWM_TIM=UH_PWM_GetTIM(channelindex,&_channelindex);

	UH_PWM_XiuGaiJiCunQi((int*)&PWM_TIM->CCR1,_channelindex,0);
 	SET_BIT(PWM_TIM->CCER,TIM_CCER_CCE(_channelindex));
	if (LL_TIM_IsEnabledCounter(PWM_TIM)==0)
	{
		LL_TIM_EnableCounter(PWM_TIM);
	}
}

void UH_PWM_Stop(unsigned int channelindex)
{
	unsigned int _channelindex;
	TIM_TypeDef * PWM_TIM=UH_PWM_GetTIM(channelindex,&_channelindex);

	CLEAR_BIT(PWM_TIM->CCER, TIM_CCER_CCE(_channelindex));
	if ((PWM_TIM->CCER&TIM_CCER_CCE_ALL)==0)
	{
		LL_TIM_DisableCounter(PWM_TIM);
	}
}

inline void UH_PWM_SetMaiKuan(unsigned int channelindex,unsigned int maikuan)
{
	unsigned int _channelindex;
	TIM_TypeDef * PWM_TIM=UH_PWM_GetTIM(channelindex,&_channelindex);
	UH_PWM_XiuGaiJiCunQi((int*)&PWM_TIM->CCR1,_channelindex,maikuan);
}

inline unsigned int UH_PWM_GetMaiKuan(unsigned int channelindex)
{
	unsigned int _channelindex;
	TIM_TypeDef * PWM_TIM=UH_PWM_GetTIM(channelindex,&_channelindex);
	
	int tmp=*((int*)&PWM_TIM->CCR1+_channelindex);
	
	return tmp;
}

inline unsigned int UH_PWM_GetJingDu(unsigned int channelindex)
{
	unsigned int _channelindex;
	TIM_TypeDef * PWM_TIM=UH_PWM_GetTIM(channelindex,&_channelindex);
	
	int tmp=*((int*)&PWM_TIM->ARR);
	
	return tmp;
}

inline void UH_PWM_XiuGaiJiCunQi(int* RBase,const int Offset,const int Value)
{
	RBase+=Offset;
	*RBase=Value;
}

