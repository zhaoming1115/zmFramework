/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： HAL_Config.h 
开发日期：2025-04-09 
文件功能：芯片配置文件。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __H_A_L__CONFIG_H_
#define __H_A_L__CONFIG_H_

/**************************************** 芯片文件**************************************/
#include "stm32f1xx.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_bus.h"
#include "HAL_DataType.h"
#include "usart.h"

extern char Image$$ER_IROM1$$Base[];
extern char Image$$ER_IROM1$$Limit[];
extern char Image$$RW_IRAM1$$Base[];
extern char Image$$RW_RAM1$$RW$$Limit[];
extern char Image$$RW_IRAM1$$RW$$Base[];
extern char Image$$RW_RAM1$$ZI$$Limit[];
extern char Image$$RW_RAM1$$ZI$$Base[];

/**************************************** 驱动接口文件**************************************/


#define ZHL_ToString0(Hong)		#Hong
#define ZHL_ToString(Hong)		ZHL_ToString0(Hong)


/*++++++++++++++++++++++++++ 系统配置 ++++++++++++++++++++++++++*/
#ifndef __weak
#define __weak   __attribute__((weak))
#endif /* __weak */
#define KB											1024
#define MB											(1024*KB)
#ifndef ZHL_uTickInMs
#define ZHL_uTickInMs									1000		//1ms中的微秒时戳数量，考虑到不同的单片机及时钟源，这个值并不一定是1000，也可能是1024或者100
#endif

//#define ZHL_MaxTickDivLevel								3

#if ZHL_uTickInMs==1024
#define ZHL_uTickToms(uTick)		((uTick)>>10)
#define ZHL_msTouTick(ms)			((ms)<<10)
#else
#define ZHL_uTickToms(uTick)		((uTick)/ZHL_uTickInMs)
#define ZHL_msTouTick(ms)			((ms)*ZHL_uTickInMs)
#endif

#define ZHL_CLOCK_JingDu							30			//系统时钟精度，单位：ppm
/*系统时钟的最大动态调整值，单位：uTick  */
#define ZHL_CLOCK_MaxTiaoZHengMicroSecond			ZHL_msTouTick(10)

#define __NoInitRAM									__attribute__((section(".bss.NO_INIT")))					//非初始化内存标志，
#define __InitRAM									__attribute__((section(".ARM.__at_0x20000000")))			//初始化内存标志

//#define __AddressOfProgramInfo						__attribute__((section(".ARM.__at_0x08000300")))	//程序信息偏移地址，包括程序版本、发布日期等，此项用于Boot程序访问
#define ZHL_VectorTable_Remap(Address)				SCB->VTOR = Address & SCB_VTOR_TBLOFF_Msk
#define ZHL_AddrIsReadOnly(Address)					((Address & 0xf0000000)==0)

#define ZHL_UseDateTime			
#define __ThreadTimer_HANDLER						TIM6_IRQHandler
#define ZHL_ThreadTimer_IRQPriority					15							//定时任务中断优先级

/*-------------------------- 系统配置 --------------------------*/

/*++++++++++++++++++++++++++ RAM配置 ++++++++++++++++++++++++++*/
#define ZHL_SRAM_BASE								0X20000000                        
#define ZHL_SRAM_SIZE								(20*KB)                         
#define ZHL_SRAM_USERBASE							ZHL_SRAM_BASE                        
#define ZHL_SRAM_USERSIZE							(ZHL_SRAM_SIZE-0X100)                        
#define ZHL_SRAM_BootParmAddress					(ZHL_SRAM_BASE+ZHL_SRAM_SIZE-4)					                        
#define ZHL_SRAM_BootParmSize						0X4
#ifdef ZHL_SRAM_HardFaultInfoBufAddr
#define ZHL_SRAM_DateTimeAddr						(ZHL_SRAM_HardFaultInfoBufAddr+ZHL_SRAM_HardFaultInfoBufSize)					                        
#else
#define ZHL_SRAM_DateTimeAddr						(ZHL_SRAM_USERSIZE)					                        

#endif
//#define __AddressOfBootParm							__attribute__((section(".ARM.__at_0x20000000")))		//boot程序与app程序共享内存地址
/*-------------------------- RAM配置 --------------------------*/

/*++++++++++++++++++++++++++ Flash配置 ++++++++++++++++++++++++++*/
#if ExFlash
#define ZHL_FLASH_UseExFlash						1			//外部Flash开关
#endif

#define ZHL_FLASH_BASE								0X08000000			//Flash基址
#define ZHL_FLASH_SIZE								(128*KB)			//Flash基址
#define ZHL_FLASH_PageSizeLevel						10					//页大小等级，基2，也就是最小擦除单元
#define ZHL_FLASH_PageSize							(1<<ZHL_FLASH_PageSizeLevel)					//页大小，也就是最小擦除单元

#define ZHL_FLASH_BootSize							0
#define ZHL_FLASH_APPDomainSize						(128*KB)		//APP程序域大小
#define ZHL_FLASH_ProgramInfoAddress				0x08000300
#define ZHL_FLASH_EWProtectSize						(0*KB)						//擦写保护区域大小，从头开始计算
#define ZHL_FLASH_StorageBASE						0X0803A000						//用户配置信息存储地址,占用最后1个扇区	
#define ZHL_FLASH_EnableReadProtect					1

#define ZHL_FLASH_StorageEND						(ZHL_FLASH_BASE+ZHL_FLASH_SIZE-1)						//用户配置信息存储区结束地址

#define ZHL_FLASH_FactoryBASE						0X0801f800		//工厂信息存储地址

#define ZHL_ProgramInfoOffsetAddress				(0x300)				//__AddressOfProgramInfo相对于ZHL_FLASH_BASE的偏移地址
#define ZHL_FLASH_APPBASE							(ZHL_FLASH_BASE+ZHL_FLASH_BootSize)
#define ZHL_FLASH_APPSIZE							ZHL_FLASH_APPDomainSize	//APP大小

#define __AddressOfElementInfo						__attribute__((section("ElementInfo")))					//
#define ZHL_FLASH_EWProtectUnitLevel				(ZHL_FLASH_PageSizeLevel+1)						//擦写保护单元大小等级，基2
#define ZHL_FLASH_EWProtectUnitSize					(1<<ZHL_FLASH_EWProtectUnitLevel)						//擦写保护单元大小

#ifdef ZHL_FLASH_UseExFlash
#define ZHL_SPIFLASH_BASE							0X08400000			//Flash基址
#define ZHL_SPIFLASH_SIZE							(2*MB)			//Flash基址
#define ZHL_SPIFLASH_PageSize						(4*KB)					//页大小，也就是最小擦除单元
#define ZHL_SPIFLASH_StorageBASE					(ZHL_SPIFLASH_BASE+120*KB)			//外部FLASH用于用户存储的起始地址
//#define ZHL_FLASH_AppBackupAddress					ZHL_SPIFLASH_BASE	

#else
//#define ZHL_FLASH_AppBackupAddress					(ZHL_FLASH_BASE+124*KB)	

#endif
#define ZHL_FLASH_AppBackupAddress					(ZHL_FLASH_APPBASE+ZHL_FLASH_APPDomainSize)	

//Flash存储区的基地址，页对齐				
//#define ZHL_FLASH_StorageBASE						(((unsigned int)Image$$ER_IROM1$$Limit+1+ZHL_FLASH_PageSize)&~(ZHL_FLASH_PageSize-1))		//预编译不可用

/*-------------------------- Flash配置 --------------------------*/

/*++++++++++++++++++++++++++ DMA配置 ++++++++++++++++++++++++++*/
#define ZHL_DMA_CHannelsPerDMA						7			//每个DMA的通道数
#define ZHL_DMA_CHannelCount						12			//DMA通道总数

/*-------------------------- Flash配置 --------------------------*/

/*++++++++++++++++++++++++++ GPIO配置 ++++++++++++++++++++++++++
GPIO使用整数索引法
编号方法：PA0为0，PA1为1....PB0为16，PB1为17,以此类推
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define ZHL_GPIO_UseFastSetMode						1

#define ZHL_GPIO_None								0xff
#define ZHL_GPIO_PortCount							5
#define ZHL_GPIO_PinCount							(16*ZHL_GPIO_PortCount)
#define ZHL_GPIO_IsValidPin(GpioID)					(GpioID>=0&&GpioID<ZHL_GPIO_PinCount)
#define ZHL_GPIO_IsValidPort(PortID)				(PortID>=0&&PortID<ZHL_GPIO_PortCount)
#define ZHL_GPIO_CHeckPin(PinID,Ret)				if(!ZHL_GPIO_IsValidPin(PinID)) return Ret;
#define ZHL_GPIO_CHeckPort(PortID,Ret)				if(!ZHL_GPIO_IsValidPort(PortID)) return Ret;

#ifndef BootProgram
#define ZHL_GPIO_ExITPins							0,1,2*16+8,2*16+9		//使用外部中断的引脚，如果不使用外部中断，建议取消宏定义，可以节省RAM和Flash
#else
#define GPIO_NoUseExIT								1
#endif

#define ZHL_GPIO_ExITIRQPriority_Normal				7		//外部中断默认优先级
#define ZHL_GPIO_ExITIRQPriority_High				1		//外部中断默认高优先级

/*-------------------------- GPIO配置 --------------------------*/


/*++++++++++++++++++++++++++ Uart配置 ++++++++++++++++++++++++++*/
#if BSPMode==1
#define ZHL_DebugUart								USART1

#else
#define ZHL_DebugUart								UART5
#endif
#ifdef UART4
#define ZHL_Uart_Index_Debug						((ZHL_DebugUart==UART5)?4: \
													(ZHL_DebugUart==UART4)?3: \
													(ZHL_DebugUart==USART3)?2: \
													(ZHL_DebugUart==USART2)?1:0)			//调试串口编号
#else
#define ZHL_Uart_Index_Debug						((ZHL_DebugUart==USART3)?2: \
													(ZHL_DebugUart==USART2)?1:0)			//调试串口编号

#endif
#define ZHL_Uart_Count								5	//Uart数量，Uart编号从0开始，其对应单片机中的第一个异步串口。此异步串口的数量必须大于使用的最大的Uart编号
#define ZHL_CDC_Count								0	//CDC虚拟串口数量，CDC驱动内编号从0开始，对应的串口PAL层设备编号从ZHL_Uart_Count开始
/*
串口的485控制引脚配置和接收指示灯引脚配置。
如果需要相应的功能，需要放开相应的宏定义，并且在相应的位置填写相应的GPIO引脚编号
配置格式为：Uart0,Uart1,..,Uartn
有几个串口定义几个引脚，中间用英文逗号隔开，如果相应的串口不需要这个功能，则配置为 ZHL_GPIO_None;
如果所有的串口都不需要相应的功能，则可以屏蔽相应的宏定义
*/

#define ZHL_Uart_Use485								1
#define ZHL_Uart_UseRxLED							1
#define ZHL_Uart_UseTxLED							0

#define ZHL_Uart_IRQPriority						2							//串口中断优先级
#define ZHL_Uart_DMAIRQPriority						ZHL_Uart_IRQPriority		//串口DMA中断优先级

extern const s_UartAutoContrlParm_t G_UartAutoContrlParmS[ZHL_Uart_Count];

/*-------------------------- Uart配置 --------------------------*/


/*++++++++++++++++++++++++++ SPI配置 ++++++++++++++++++++++++++*/
#define ZHL_SPI_Count								3
#define ZHL_SPI_IsValidPort(SpiID)					(SpiID>=0&&SpiID<ZHL_SPI_Count)
//#define	ZHL_SPI_SlaveCSCHeckPins					ZHL_GPIO_None,3*16+10,2*16+12
//#define	ZHL_SPI_SlaveCSCHeckPins					ZHL_GPIO_None,ZHL_GPIO_None,2*16+9		//SPI从机空闲中断检测引脚
#define	ZHL_SPI_SlaveCSCHeckPins					ZHL_GPIO_None,ZHL_GPIO_None,ZHL_GPIO_None		//SPI从机空闲中断检测引脚
//#define	ZHL_SPI_SlaveCSCHeckPins					ZHL_GPIO_None,3*16+10,ZHL_GPIO_None

/*-------------------------- SPI配置 --------------------------*/

/*++++++++++++++++++++++++ ADC配置 ++++++++++++++++++++++++++++++++++++++++++++++*/
#define ZHL_ADC_SEG_Count							0		//序列通道数量
#define ZHL_ADC_INJ_Count							1		//注册通道数量
#define ZHL_ADC_IsINJCHannel(CHannelIndex)			(CHannelIndex>=ZHL_ADC_SEG_Count)

/*-------------------------ADC配置 ---------------------------------------------*/

/*++++++++++++++++++++++++ I2C +++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define ZHL_I2C_Count								2
#define ZHL_I2C_IRQPriority							2							//中断优先级
extern const unsigned char G_I2CGPIOGroupS[ZHL_I2C_Count];

/*-------------------------I2C ---------------------------------------------*/

/*+++++++++++++++++++++++++  USB配置 ++++++++++++++++++++++++++++++++++++++++++++++*/

/*-----------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++寄存器操作++++++++++++++++++++++++++*/
#define ZHL_SetBit(reg, bit)						((reg) |= (1 << (bit)))
#define ZHL_ClrBit(reg, bit)						((reg) &= (~(1 << (bit))))
#define ZHL_GetBit(reg, bit)						((reg) & (1 << (bit)))
#define ZHL_GetBits(reg, bits)						((reg) & bits)
#define ZHL_SetBits(reg, bits)						((reg) |= bits)
#define ZHL_ClrBits(reg, bit)						((reg) &= (~(bit)))

#define ZHL_GetArrayCount(Array)					(sizeof(Array)/sizeof(Array[0]))

/*--------------------------寄存器操作--------------------------*/

#endif