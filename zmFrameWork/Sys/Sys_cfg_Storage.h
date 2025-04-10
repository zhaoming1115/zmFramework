/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� Sys_cfg_Storage.h 
�������ڣ�2025-04-09 
�ļ����ܣ�������洢����ĵ�ַ�ʹ�С��

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/

 #ifndef __SYS_CFG__STORAGE_H_
#define __SYS_CFG__STORAGE_H_

#ifndef KB
#define KB		1024
#define MB		(1024*KB)
#endif

#define Sys_InnerFlash_EraseValue					0xff
#if DebugMode
#define Sys_ProgramInfoAddress						0x08000300

#else
#define Sys_ProgramInfoAddress						0x08004300

#endif
#define __AddressOfProgramInfo						__attribute__((section(".ARM.__at_"HongToString(Sys_ProgramInfoAddress))))	//������Ϣƫ�Ƶ�ַ����������汾���������ڵȣ���������Boot�������

#define Sys_StorageBase_InnerFlash					0x08000000
#define Sys_StorageSize_InnerFlash					(256*KB)
#define Sys_StorageBlock_InnerFlash					(2*KB)

#define Sys_StorageAddress_Program					Sys_StorageBase_InnerFlash
#define Sys_StorageSize_App							(108*KB)
#define Sys_StorageSize_Program						((16*KB)+Sys_StorageSize_App)
#define Sys_StorageAddress_TmpBuffer				(Sys_StorageBase_InnerFlash+Sys_StorageSize_Program)

#define Sys_StorageAddress_Factory					(Sys_StorageBase_InnerFlash+Sys_StorageSize_InnerFlash-Sys_StorageBlock_InnerFlash)
#define Sys_StorageAddress_Config					(Sys_StorageAddress_Factory-Sys_StorageBlock_InnerFlash)

#define Sys_StorageSize_ElementInfo					(4*KB)
#define Sys_StorageAddress_ElementInfo				(0x0803A000)


//#define Sys_StorageAddress_ElementInfo				(Sys_StorageAddress_Config-Sys_StorageSize_ElementInfo)
#define __AddressOfElementInfo						__attribute__((section("ElementInfo")))	//Ԫ����Ϣ��ַ

#endif


 

