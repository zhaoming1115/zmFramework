/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： UH_GPIO.c 
开发日期：2025-04-09 
文件功能：

描    述：

依赖接口：

提供接口：
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/
#include "../Inc/UH_GPIO.h"

#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_exti.h"
#include "gpio.h"

#ifndef assert_param
#define 	assert_param(x)     
#endif


#ifdef GPIOG
static GPIO_TypeDef* const GPIOPortTable[]={GPIOA,GPIOB,GPIOC,GPIOD,GPIOE,GPIOF,GPIOG};

#else
static GPIO_TypeDef* const GPIOPortTable[]={GPIOA,GPIOB,GPIOC,GPIOD,GPIOE};

#endif

static unsigned int const GPIOPinTable[]={LL_GPIO_PIN_0,LL_GPIO_PIN_1,LL_GPIO_PIN_2,LL_GPIO_PIN_3,
	LL_GPIO_PIN_4,LL_GPIO_PIN_5,LL_GPIO_PIN_6,LL_GPIO_PIN_7,
	LL_GPIO_PIN_8,LL_GPIO_PIN_9,LL_GPIO_PIN_10,LL_GPIO_PIN_11,
	LL_GPIO_PIN_12,LL_GPIO_PIN_13,LL_GPIO_PIN_14,LL_GPIO_PIN_15};


#define GPIO_GetPort(GPIOx)			(GPIOPortTable[GPIOx/16])
#define GPIO_GetPin(GPIOx)			(GPIOPinTable[GPIOx%16])

int UH_GPIO_SetMode(gpio_t GPIOx,e_GPIO_Mode_t mode)
{
	ZHL_GPIO_CHeckPin(GPIOx,GPIO_Err_NoPin);
	if(mode & GPIO_MD_FuYongFlag)
	{
		LL_GPIO_SetPinMode(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),LL_GPIO_MODE_ALTERNATE);
		LL_GPIO_SetPinSpeed(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),LL_GPIO_SPEED_FREQ_HIGH);
		LL_GPIO_SetPinOutputType(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),((mode&GPIO_MD_TypeMak)==GPIO_MD_PushPull)?LL_GPIO_OUTPUT_PUSHPULL:LL_GPIO_OUTPUT_OPENDRAIN);
	}
	else
	{
		switch(mode&GPIO_MD_TypeMak)
		{
		case GPIO_MD_None:
			LL_GPIO_SetPinMode(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),LL_GPIO_MODE_ANALOG);
			break;

		case GPIO_MD_Input:
			LL_GPIO_SetPinMode(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),(mode&GPIO_MD_PullMak)?LL_GPIO_MODE_INPUT:LL_GPIO_MODE_FLOATING);
			break;

		case GPIO_MD_OD:
			LL_GPIO_SetPinMode(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),LL_GPIO_MODE_OUTPUT);
			LL_GPIO_SetPinSpeed(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),LL_GPIO_SPEED_FREQ_HIGH);
			LL_GPIO_SetPinOutputType(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),LL_GPIO_OUTPUT_OPENDRAIN);
			break;

		case GPIO_MD_PushPull:
			LL_GPIO_SetPinMode(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),LL_GPIO_MODE_OUTPUT);
			LL_GPIO_SetPinSpeed(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),LL_GPIO_SPEED_FREQ_HIGH);
			LL_GPIO_SetPinOutputType(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),LL_GPIO_OUTPUT_PUSHPULL);
			break;

		default:
			return GPIO_Err_ModeIsNoSurport;
			break;
		}
	}
	switch(mode&GPIO_MD_PullMak)
	{
	case GPIO_MD_PullUp:
		LL_GPIO_SetPinPull(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),LL_GPIO_PULL_UP);
		break;

	case GPIO_MD_PullDown:
		LL_GPIO_SetPinPull(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),LL_GPIO_PULL_DOWN);
		break;

	default:
#ifdef LL_GPIO_PULL_NO
		LL_GPIO_SetPinPull(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),LL_GPIO_PULL_NO);
#endif
		break;
	}
	return GPIO_Err_None;
}

int UH_GPIO_SetExitIT(gpio_t GPIOx,e_Exit_TriggerMode_t TriggerMode)
{
#if	!GPIO_NoUseExIT	
	static const char ExitTriggerModeTable[]={LL_EXTI_TRIGGER_NONE,LL_EXTI_TRIGGER_RISING,LL_EXTI_TRIGGER_FALLING,LL_EXTI_TRIGGER_RISING_FALLING};
	UH_GPIO_SetMode(GPIOx,GPIO_MD_Input);
	
	int PortIndex=GPIOx/16;
	GPIOx%=16;	
	if(LL_EXTI_IsEnabledIT_0_31(GPIOx)) return GPIO_Err_ModeIsNoSurport;

	int REG=GPIOx/4;
	int pos=(GPIOx%4) * 4;	
	MODIFY_REG(AFIO->EXTICR[REG],0xf<<pos,PortIndex<<pos);		//配置外部中断输入源

	LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

//	LL_SYSCFG_SetEXTISource(ExitITSourcePort[ExitNum], ExitITSourceLine[ExitNum]);
	EXTI_InitStruct.Line_0_31 = 1<<GPIOx;
	EXTI_InitStruct.LineCommand = ENABLE;
	EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
	EXTI_InitStruct.Trigger = ExitTriggerModeTable[TriggerMode & Exit_TMD_Mak];
	LL_EXTI_Init(&EXTI_InitStruct);

	
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0<<GPIOx);
	if(GPIOx<5)
	{
		GPIOx+=EXTI0_IRQn;
	}
	else if(GPIOx<10)
	{
		GPIOx=EXTI9_5_IRQn;
	}
	else
	{
		GPIOx=EXTI15_10_IRQn;
	}
	
	if(TriggerMode & Exit_YXJ_High)
	{
		NVIC_SetPriority(GPIOx, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),ZHL_GPIO_ExITIRQPriority_Normal, 0));
	}
	else
	{
		NVIC_SetPriority(GPIOx, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),ZHL_GPIO_ExITIRQPriority_High, 0));
	}
	NVIC_EnableIRQ(GPIOx);
	return 0;
#else
	return GPIO_Err_ModeIsNoSurport;
#endif
}

inline int UH_GPIO_ClearExitIT(gpio_t GPIOx)
{
#if	!GPIO_NoUseExIT	
	LL_EXTI_DisableIT_0_31(1<<(GPIOx%16));
	return 0;
#else
	return GPIO_Err_ModeIsNoSurport;
#endif
}

inline void UH_GPIO_Init(void)
{
//	MX_GPIO_Init();
	 LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA|LL_APB2_GRP1_PERIPH_GPIOB|LL_APB2_GRP1_PERIPH_GPIOC|LL_APB2_GRP1_PERIPH_AFIO);

}

inline void UH_GPIO_Set(size_t GPIOx)
{
	ZHL_GPIO_CHeckPin(GPIOx,);
	LL_GPIO_SetOutputPin(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx));
}

inline void UH_GPIO_Reset(size_t GPIOx)
{
	ZHL_GPIO_CHeckPin(GPIOx,);
	LL_GPIO_ResetOutputPin(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx));
}

e_GPIO_Value_t UH_GPIO_ReadPin(gpio_t GPIOx)
{
	ZHL_GPIO_CHeckPin(GPIOx,GPIO_Value_LowLevel);
	if (LL_GPIO_GetPinMode(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx))==LL_GPIO_MODE_INPUT)
	{
		return LL_GPIO_IsInputPinSet(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx));
	}
	else
	{
		return LL_GPIO_IsOutputPinSet(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx));
	}
}

int UH_GPIO_WritePin(gpio_t GPIOx,e_GPIO_Value_t Value)
{
	ZHL_GPIO_CHeckPin(GPIOx,GPIO_Err_NoPin);
	typeof(GPIOPortTable[0]) GPIOPort=GPIO_GetPort(GPIOx);
	typeof(GPIOPinTable[0]) Pin=GPIO_GetPin(GPIOx);
	switch(Value)
	{
		case GPIO_Value_LowLevel:
			LL_GPIO_ResetOutputPin(GPIOPort,Pin);
			break;

		case GPIO_Value_HighLevel:
			LL_GPIO_SetOutputPin(GPIOPort,Pin);
			break;

		case GPIO_Value_CHange:
			LL_GPIO_TogglePin(GPIOPort,Pin);
			break;

		default:
			return GPIO_Err_ModeIsNoSurport;
			break;
	}
	return GPIO_Err_None;	
}

#ifdef ZHL_GPIO_ExITPins
static void UH_GPIO_ExITHandler(int ExITNum)
{
	unsigned int REG=ExITNum/4;
	unsigned int pos=(ExITNum%4) * 4;	
	unsigned int GPIOGroupID=(AFIO->EXTICR[REG]>>pos) & 0XF;
	UHC_GPIO_InHandler(GPIOGroupID * 16 + ExITNum);
}

void EXTI0_IRQHandler(void)
{
	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0) != RESET)
	{
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
		UH_GPIO_ExITHandler(0);
	}
}

void EXTI1_IRQHandler(void)
{
	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_1) != RESET)
	{
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1);
		UH_GPIO_ExITHandler(1);
	}
}

void EXTI2_IRQHandler(void)
{
	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_2) != RESET)
	{
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
		UH_GPIO_ExITHandler(2);
	}
}

void EXTI3_IRQHandler(void)
{
	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_3) != RESET)
	{
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
		UH_GPIO_ExITHandler(3);
	}
}

void EXTI4_IRQHandler(void)
{
	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_4) != RESET)
	{
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
		UH_GPIO_ExITHandler(4);
	}
}

void EXTI9_5_IRQHandler(void)
{
	int tmp;
	for(int i=5;i<10;i++)
	{
		tmp=1<<i;
		if (LL_EXTI_IsActiveFlag_0_31(tmp) != RESET)
		{
			LL_EXTI_ClearFlag_0_31(tmp);
			UH_GPIO_ExITHandler(i);
		}
	}
	LL_EXTI_ClearFlag_0_31(0b1111100000);
}

void EXTI15_10_IRQHandler(void)
{
	int tmp;
	for(int i=10;i<16;i++)
	{
		tmp=1<<i;
		if (LL_EXTI_IsActiveFlag_0_31(tmp) != RESET)
		{
			LL_EXTI_ClearFlag_0_31(tmp);
			UH_GPIO_ExITHandler(i);
		}
	}
	LL_EXTI_ClearFlag_0_31(0b1111110000000000);
}
#endif

#if ZHL_GPIO_UseFastSetMode
void UH_GPIO_SetInMode(gpio_t GPIOx)
{
	LL_GPIO_SetPinMode(GPIO_GetPort(GPIOx),GPIO_GetPin(GPIOx),LL_GPIO_MODE_INPUT);	
}

void UH_GPIO_SetODMode(gpio_t GPIOx)
{
	GPIO_TypeDef* _PORT=GPIO_GetPort(GPIOx);
	unsigned int _PIN=GPIO_GetPin(GPIOx);

	LL_GPIO_SetPinMode(_PORT,_PIN,LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(_PORT,_PIN,LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(_PORT,_PIN,LL_GPIO_OUTPUT_OPENDRAIN);
}
#endif

