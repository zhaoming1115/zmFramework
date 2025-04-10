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
/*���ļ�����PLOOC���ƣ�������ԭ��˵��*/
/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� HAL_Interface_Storage.h 
�������ڣ�2024-04-14 
�ļ����ܣ���ӦC�ļ���ͷ�ļ�������C�ļ����⹫�����������͡��ӿں�����ȫ�ֱ�����

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/


 #ifndef __H_A_L__INTERFACE__STORAGE_H_
#define __H_A_L__INTERFACE__STORAGE_H_

#define __Storage_StateCHeckOnPAL		1
#ifndef BootProgram
#define Storage_UseBitRW				1
#endif

typedef enum
{
	Storage_Type_ExFLASH=-2,		//�ⲿFLASH
	Storage_Type_ExEPPROM=-1,		//�ⲿEPPROM
	Storage_Type_InnerEPPROM,		//�ڲ�EPPROM
	Storage_Type_InnerFlash,		//�ڲ��洢��
	Storage_Type_ExFlashAsInner,			//�ⲿ�洢�������ǿ������ڲ��洢��һ��ʹ��
}e_StorageType_t;	//�洢������

#define Storage_IsWithSysLine(Type)	(Type>=0)			//�洢���Ƿ�ʹ��ϵͳ���߷��ʣ�Ҳ����֧�ֵ�ֱַ��

#define Storage_NoSurportLevel				0xff
#define Storage_IsVaildLevel(__Level)		(__Level>0 && __Level<16)

#if     defined(__BlockStorage_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__BlockStorage_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"

declare_interface(i_StorageBlock_t)

//�洢�������ÿ�������Ĵ洢���鶼Ӧ��ʵ����һ���ö���
declare_class(c_StorageBlock_t)
def_class(c_StorageBlock_t,

    public_member(
        //!< place your public members here
		const i_StorageBlock_t* const Manager;
		e_StorageType_t StorageType;
		char EraseValue;						//�������ֵ,����Ϊ�з����ͣ�
		unsigned char WriteUnitLevel;		//д�뵥Ԫ�Ĵ�С�ȼ�����2���磺0-1�ֽ�,1-2�ֽڣ�2-4�ֽڡ�������
		unsigned char EraseUnitLevel;		//������Ԫ�Ĵ�С�ȼ�����2���磺0-1�ֽڣ�1-2�ֽڣ�2-4�ֽڡ�������
	
		unsigned char ClearBitsLevel;		//���λ���ȼ���0-1bit��1-2bit��3-1�ֽڣ�5-4�ֽڣ�0xff-��֧�����λ
		unsigned char ReadbufSizeLevel;		//����������С�ȼ�����2,0xff-��ʹ�ö�������
		unsigned char MaxEraseTime;			//����һ����С��Ԫ��Ҫ�����ʱ�䣬��λ��ms

		unsigned int BaseAddress;
		unsigned int Size;
		
    )

    private_member(
        //!< place your private members here
    )
    
    protected_member(
        //!< place your private members here
    )
)

end_def_class(c_StorageBlock_t) /* do not remove this for forward compatibility  */

//�洢�������Ĳ����ӿڣ�ÿһ��洢���鶼Ӧ��ʵ��һ��ýӿ�
def_interface(i_StorageBlock_t)
    int (*Init)(const c_StorageBlock_t* StorageBlock);
	/*����˵������ȡ�洢�豸������
	����˵����
	���������
	Address-�洢�豸������������ַ
	*ToData-Ҫ��ȡ�����ڴ�ָ��
	ByteCount-Ҫ��ȡ���ֽ���
	���أ���ToDataΪ�գ����ȡ���������У������ػ������ĵ�ַ�� 
		����>=0-ʵ�ʶ�ȡ��������<0-�쳣
	*/
	int (*Read)(const c_StorageBlock_t* StorageBlock,unsigned int Address,char* ToData,unsigned int ByteCount);

	/*����˵������ָ������������д��洢�豸
	����˵����
	���������
	Address-Ҫд��ĵ�ַ
	*FromData-������Դ�ڴ�ָ��
	ByteCount-Ҫд����ֽ���
	���أ�>=0-ʵ��д���������<0-�쳣
	*/
	int (*WriteUnit)(const c_StorageBlock_t* StorageBlock,unsigned int Address, const int* FromData);
	/*����˵��������ָ����ַ���ڵ���������ҳ
	����˵����
	���������
	Address-Ҫ�����������ʼ��ַ
	���أ�>0��ʵ�ʲ����������С��<=0:ʧ��
	*/
	int (*EraseUnit)(const c_StorageBlock_t* StorageBlock,unsigned int Address);
	bool (*IsBusy)(const c_StorageBlock_t* StorageBlock,unsigned int Address);

#if Storage_UseBitRW
//	int (*ReadBitUnit)(const c_StorageBlock_t* StorageBlock,unsigned int BitOffsetAddress, int* ToData);
	int (*WriteBitUnit)(const c_StorageBlock_t* StorageBlock,unsigned int BitOffsetAddress, const char* FromData);
#endif
	bool (*DisableProtect)(const c_StorageBlock_t* StorageBlock,unsigned int Address,unsigned int Size,bool RDPFlag);
	bool (*EnableProtect)(const c_StorageBlock_t* StorageBlock,unsigned int Address,unsigned int Size);
	bool (*IsEWProtect)(const c_StorageBlock_t* StorageBlock,unsigned int Address,unsigned int Size);

end_def_interface(i_StorageBlock_t) /*do not remove this for forward compatibility */

extern void Storage_AddDevices(int (*AddStorageBlock)(const c_StorageBlock_t*  StorageBlock));

#undef __BlockStorage_CLASS_INHERIT__
#undef __BlockStorage_CLASS_IMPLEMENT__

#endif


 

