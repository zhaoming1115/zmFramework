/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： Sys_cfg.h 
开发日期：2025-04-09 
文件功能：定义系统组件需要的配置信息。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __SYS_CFG_H_
#define __SYS_CFG_H_

#include <cmsis_compiler.h>
#include "../C_Extended.h"
#include "Sys_cfg_Storage.h"

#define	Sys_Malloc(Size)		malloc(Size)
#define	Sys_Free(Object)		free(Object)
#define __NoInitRAM				__attribute__((section(".bss.NO_INIT")))				
#define __InitRAM				__attribute__((section(".ARM.__at_0x20000000")))			
#define __BootParmRAM			__attribute__((section(".bss.BootParm")))					

#define Sys_PrintError			printf

#define SYS_EVAL(__NAME,...)	 __PLOOC_EVAL(__NAME, __VA_ARGS__)
	
#define Debug_Level				1


inline static int Sys_DisableIRQ(void)
{
    int tStatus = __get_PRIMASK();
    __disable_irq();  
    return tStatus;
}

inline static void Sys_EnableIRQ(int tStatus)
{
    __set_PRIMASK(tStatus);
}

#ifndef __IRQ_SAFE
#define __IRQ_SAFE  using( int SAFE_NAME(temp) =           \
                        Sys_DisableIRQ(),                  \
                    Sys_EnableIRQ(SAFE_NAME(temp)))

#endif
#define Queue_SafeCode				__IRQ_SAFE
#define List_SafeCode				__IRQ_SAFE

#define Thread_TickSizeUse64bit		0
#define Thread_UseTimer				1

#endif
