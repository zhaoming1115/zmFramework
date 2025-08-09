/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� HAL_Config.h 
�������ڣ�2025-04-09 
�ļ����ܣ�оƬ�����ļ���

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/

 #ifndef __H_A_L__CONFIG_H_
#define __H_A_L__CONFIG_H_

/**************************************** оƬ�ļ�**************************************/
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

/**************************************** �����ӿ��ļ�**************************************/


#define ZHL_ToString0(Hong)		#Hong
#define ZHL_ToString(Hong)		ZHL_ToString0(Hong)


/*++++++++++++++++++++++++++ ϵͳ���� ++++++++++++++++++++++++++*/
#ifndef __weak
#define __weak   __attribute__((weak))
#endif /* __weak */
#define KB											1024
#define MB											(1024*KB)
#ifndef ZHL_uTickInMs
#define ZHL_uTickInMs									1000		//1ms�е�΢��ʱ�����������ǵ���ͬ�ĵ�Ƭ����ʱ��Դ�����ֵ����һ����1000��Ҳ������1024����100
#endif

//#define ZHL_MaxTickDivLevel								3

#if ZHL_uTickInMs==1024
#define ZHL_uTickToms(uTick)		((uTick)>>10)
#define ZHL_msTouTick(ms)			((ms)<<10)
#else
#define ZHL_uTickToms(uTick)		((uTick)/ZHL_uTickInMs)
#define ZHL_msTouTick(ms)			((ms)*ZHL_uTickInMs)
#endif

#define ZHL_CLOCK_JingDu							30			//ϵͳʱ�Ӿ��ȣ���λ��ppm
/*ϵͳʱ�ӵ����̬����ֵ����λ��uTick  */
#define ZHL_CLOCK_MaxTiaoZHengMicroSecond			ZHL_msTouTick(10)

#define __NoInitRAM									__attribute__((section(".bss.NO_INIT")))					//�ǳ�ʼ���ڴ��־��
#define __InitRAM									__attribute__((section(".ARM.__at_0x20000000")))			//��ʼ���ڴ��־

//#define __AddressOfProgramInfo						__attribute__((section(".ARM.__at_0x08000300")))	//������Ϣƫ�Ƶ�ַ����������汾���������ڵȣ���������Boot�������
#define ZHL_VectorTable_Remap(Address)				SCB->VTOR = Address & SCB_VTOR_TBLOFF_Msk
#define ZHL_AddrIsReadOnly(Address)					((Address & 0xf0000000)==0)

#define ZHL_UseDateTime			
#define __ThreadTimer_HANDLER						TIM6_IRQHandler
#define ZHL_ThreadTimer_IRQPriority					15							//��ʱ�����ж����ȼ�

/*-------------------------- ϵͳ���� --------------------------*/

/*++++++++++++++++++++++++++ RAM���� ++++++++++++++++++++++++++*/
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
//#define __AddressOfBootParm							__attribute__((section(".ARM.__at_0x20000000")))		//boot������app�������ڴ��ַ
/*-------------------------- RAM���� --------------------------*/

/*++++++++++++++++++++++++++ Flash���� ++++++++++++++++++++++++++*/
#if ExFlash
#define ZHL_FLASH_UseExFlash						1			//�ⲿFlash����
#endif

#define ZHL_FLASH_BASE								0X08000000			//Flash��ַ
#define ZHL_FLASH_SIZE								(128*KB)			//Flash��ַ
#define ZHL_FLASH_PageSizeLevel						10					//ҳ��С�ȼ�����2��Ҳ������С������Ԫ
#define ZHL_FLASH_PageSize							(1<<ZHL_FLASH_PageSizeLevel)					//ҳ��С��Ҳ������С������Ԫ

#define ZHL_FLASH_BootSize							0
#define ZHL_FLASH_APPDomainSize						(128*KB)		//APP�������С
#define ZHL_FLASH_ProgramInfoAddress				0x08000300
#define ZHL_FLASH_EWProtectSize						(0*KB)						//��д���������С����ͷ��ʼ����
#define ZHL_FLASH_StorageBASE						0X0803A000						//�û�������Ϣ�洢��ַ,ռ�����1������	
#define ZHL_FLASH_EnableReadProtect					1

#define ZHL_FLASH_StorageEND						(ZHL_FLASH_BASE+ZHL_FLASH_SIZE-1)						//�û�������Ϣ�洢��������ַ

#define ZHL_FLASH_FactoryBASE						0X0801f800		//������Ϣ�洢��ַ

#define ZHL_ProgramInfoOffsetAddress				(0x300)				//__AddressOfProgramInfo�����ZHL_FLASH_BASE��ƫ�Ƶ�ַ
#define ZHL_FLASH_APPBASE							(ZHL_FLASH_BASE+ZHL_FLASH_BootSize)
#define ZHL_FLASH_APPSIZE							ZHL_FLASH_APPDomainSize	//APP��С

#define __AddressOfElementInfo						__attribute__((section("ElementInfo")))					//
#define ZHL_FLASH_EWProtectUnitLevel				(ZHL_FLASH_PageSizeLevel+1)						//��д������Ԫ��С�ȼ�����2
#define ZHL_FLASH_EWProtectUnitSize					(1<<ZHL_FLASH_EWProtectUnitLevel)						//��д������Ԫ��С

#ifdef ZHL_FLASH_UseExFlash
#define ZHL_SPIFLASH_BASE							0X08400000			//Flash��ַ
#define ZHL_SPIFLASH_SIZE							(2*MB)			//Flash��ַ
#define ZHL_SPIFLASH_PageSize						(4*KB)					//ҳ��С��Ҳ������С������Ԫ
#define ZHL_SPIFLASH_StorageBASE					(ZHL_SPIFLASH_BASE+120*KB)			//�ⲿFLASH�����û��洢����ʼ��ַ
//#define ZHL_FLASH_AppBackupAddress					ZHL_SPIFLASH_BASE	

#else
//#define ZHL_FLASH_AppBackupAddress					(ZHL_FLASH_BASE+124*KB)	

#endif
#define ZHL_FLASH_AppBackupAddress					(ZHL_FLASH_APPBASE+ZHL_FLASH_APPDomainSize)	

//Flash�洢���Ļ���ַ��ҳ����				
//#define ZHL_FLASH_StorageBASE						(((unsigned int)Image$$ER_IROM1$$Limit+1+ZHL_FLASH_PageSize)&~(ZHL_FLASH_PageSize-1))		//Ԥ���벻����

/*-------------------------- Flash���� --------------------------*/

/*++++++++++++++++++++++++++ DMA���� ++++++++++++++++++++++++++*/
#define ZHL_DMA_CHannelsPerDMA						7			//ÿ��DMA��ͨ����
#define ZHL_DMA_CHannelCount						12			//DMAͨ������

/*-------------------------- Flash���� --------------------------*/

/*++++++++++++++++++++++++++ GPIO���� ++++++++++++++++++++++++++
GPIOʹ������������
��ŷ�����PA0Ϊ0��PA1Ϊ1....PB0Ϊ16��PB1Ϊ17,�Դ�����
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
#define ZHL_GPIO_ExITPins							0,1,2*16+8,2*16+9		//ʹ���ⲿ�жϵ����ţ������ʹ���ⲿ�жϣ�����ȡ���궨�壬���Խ�ʡRAM��Flash
#else
#define GPIO_NoUseExIT								1
#endif

#define ZHL_GPIO_ExITIRQPriority_Normal				7		//�ⲿ�ж�Ĭ�����ȼ�
#define ZHL_GPIO_ExITIRQPriority_High				1		//�ⲿ�ж�Ĭ�ϸ����ȼ�

/*-------------------------- GPIO���� --------------------------*/


/*++++++++++++++++++++++++++ Uart���� ++++++++++++++++++++++++++*/
#if BSPMode==1
#define ZHL_DebugUart								USART1

#else
#define ZHL_DebugUart								UART5
#endif
#ifdef UART4
#define ZHL_Uart_Index_Debug						((ZHL_DebugUart==UART5)?4: \
													(ZHL_DebugUart==UART4)?3: \
													(ZHL_DebugUart==USART3)?2: \
													(ZHL_DebugUart==USART2)?1:0)			//���Դ��ڱ��
#else
#define ZHL_Uart_Index_Debug						((ZHL_DebugUart==USART3)?2: \
													(ZHL_DebugUart==USART2)?1:0)			//���Դ��ڱ��

#endif
#define ZHL_Uart_Count								5	//Uart������Uart��Ŵ�0��ʼ�����Ӧ��Ƭ���еĵ�һ���첽���ڡ����첽���ڵ������������ʹ�õ�����Uart���
#define ZHL_CDC_Count								0	//CDC���⴮��������CDC�����ڱ�Ŵ�0��ʼ����Ӧ�Ĵ���PAL���豸��Ŵ�ZHL_Uart_Count��ʼ
/*
���ڵ�485�����������úͽ���ָʾ���������á�
�����Ҫ��Ӧ�Ĺ��ܣ���Ҫ�ſ���Ӧ�ĺ궨�壬��������Ӧ��λ����д��Ӧ��GPIO���ű��
���ø�ʽΪ��Uart0,Uart1,..,Uartn
�м������ڶ��弸�����ţ��м���Ӣ�Ķ��Ÿ����������Ӧ�Ĵ��ڲ���Ҫ������ܣ�������Ϊ ZHL_GPIO_None;
������еĴ��ڶ�����Ҫ��Ӧ�Ĺ��ܣ������������Ӧ�ĺ궨��
*/

#define ZHL_Uart_Use485								1
#define ZHL_Uart_UseRxLED							1
#define ZHL_Uart_UseTxLED							0

#define ZHL_Uart_IRQPriority						2							//�����ж����ȼ�
#define ZHL_Uart_DMAIRQPriority						ZHL_Uart_IRQPriority		//����DMA�ж����ȼ�

extern const s_UartAutoContrlParm_t G_UartAutoContrlParmS[ZHL_Uart_Count];

/*-------------------------- Uart���� --------------------------*/


/*++++++++++++++++++++++++++ SPI���� ++++++++++++++++++++++++++*/
#define ZHL_SPI_Count								3
#define ZHL_SPI_IsValidPort(SpiID)					(SpiID>=0&&SpiID<ZHL_SPI_Count)
//#define	ZHL_SPI_SlaveCSCHeckPins					ZHL_GPIO_None,3*16+10,2*16+12
//#define	ZHL_SPI_SlaveCSCHeckPins					ZHL_GPIO_None,ZHL_GPIO_None,2*16+9		//SPI�ӻ������жϼ������
#define	ZHL_SPI_SlaveCSCHeckPins					ZHL_GPIO_None,ZHL_GPIO_None,ZHL_GPIO_None		//SPI�ӻ������жϼ������
//#define	ZHL_SPI_SlaveCSCHeckPins					ZHL_GPIO_None,3*16+10,ZHL_GPIO_None

/*-------------------------- SPI���� --------------------------*/

/*++++++++++++++++++++++++ ADC���� ++++++++++++++++++++++++++++++++++++++++++++++*/
#define ZHL_ADC_SEG_Count							0		//����ͨ������
#define ZHL_ADC_INJ_Count							1		//ע��ͨ������
#define ZHL_ADC_IsINJCHannel(CHannelIndex)			(CHannelIndex>=ZHL_ADC_SEG_Count)

/*-------------------------ADC���� ---------------------------------------------*/

/*++++++++++++++++++++++++ I2C +++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define ZHL_I2C_Count								2
#define ZHL_I2C_IRQPriority							2							//�ж����ȼ�
extern const unsigned char G_I2CGPIOGroupS[ZHL_I2C_Count];

/*-------------------------I2C ---------------------------------------------*/

/*+++++++++++++++++++++++++  USB���� ++++++++++++++++++++++++++++++++++++++++++++++*/

/*-----------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++�Ĵ�������++++++++++++++++++++++++++*/
#define ZHL_SetBit(reg, bit)						((reg) |= (1 << (bit)))
#define ZHL_ClrBit(reg, bit)						((reg) &= (~(1 << (bit))))
#define ZHL_GetBit(reg, bit)						((reg) & (1 << (bit)))
#define ZHL_GetBits(reg, bits)						((reg) & bits)
#define ZHL_SetBits(reg, bits)						((reg) |= bits)
#define ZHL_ClrBits(reg, bit)						((reg) &= (~(bit)))

#define ZHL_GetArrayCount(Array)					(sizeof(Array)/sizeof(Array[0]))

/*--------------------------�Ĵ�������--------------------------*/

#endif