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
�� �� ���� zmStorage.h 
�������ڣ�2025-03-26 
�ļ����ܣ�����洢��������ص��ࡣ

��    �����洢�������������ͳһ��������ͬ���͵Ĵ洢������ͨ����ַ�������֡�

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/


#ifndef __zmStorage_H__
#define __zmStorage_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../PAL_Config.h"
#include "Sys/Sys_DataType.h"

/*============================ MACROS ========================================*/
#define Storage_WriteAddressForFlush			0			//��д��������õ�ַʱ��ִ��FLUSH������Ŀ����Ϊ��ͨ��һ���ӿ�ʵ�ִ洢��������д�빦��

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef enum
{
	Storage_State_IDLE,
	Storage_State_Busy=!Storage_State_IDLE,
}e_StorageState_t;//�洢��״̬

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__Storage_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__Storage_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class Storage_t
//! @{
declare_class(c_Storage_t)

def_class(c_Storage_t,

    public_member(
        //!< place your public members here
    )

    private_member(
        //!< place your private members here
    )
    
    protected_member(
        //!< place your private members here
    )
)

end_def_class(c_Storage_t) /* do not remove this for forward compatibility  */
//! @}


typedef struct Storage_cfg_t {
    
    //! put your configuration members here
	unsigned char EWInfoCount;	//��д��Ϣ��������СΪ1�����ж�����ͬʱ�����洢������Ҫ�첽��дʱ���������ø�����������������Զ����������ȷ����ͬһ���洢���Ĵ��в���
} s_Storage_cfg_t;//�洢������

#define Append_StorageConfig_Default()		{1}

def_interface(i_StorageEvent_t)
	void (*BlockErasing)(unsigned int Address,int Length);						//�齫Ҫ��ʼ����
	void (*BlockErased)(unsigned int Address,int Length);						//��������
end_def_interface(i_StorageEvent_t) /*do not remove this for forward compatibility */

//! \name interface i_Storage_t
//! @{
def_interface(i_Storage_t)
    int (*Init)(const s_Storage_cfg_t *const ptCFG,const i_StorageEvent_t* const Event);
	/*����˵������ȡ�洢�豸������
	����˵����
	���������
	Address-�洢�豸������������ַ
	*ToData-Ҫ��ȡ�����ڴ�ָ��
	ByteCount-Ҫ��ȡ���ֽ���
	���أ���ToDataΪ�գ����ȡ���������У������ػ������ĵ�ַ�� 
		����>=0-ʵ�ʶ�ȡ��������<0-�쳣
	*/
	int (*Read)(unsigned int Address,char* ToData,unsigned int ByteCount);

	/*����˵������ָ������������д��洢�豸
	����˵����
	���������
	Address-Ҫд��ĵ�ַ
	*FromData-������Դ�ڴ�ָ��
	ByteCount-Ҫд����ֽ���
	���أ�>=0-ʵ��д���������<0-�쳣
	*/
	int (*Write)(unsigned int Address, const char* FromData, unsigned int ByteCount);
	/*����˵��������ָ����ַ���ڵ���������ҳ
	����˵����
	���������
	StartAddress-Ҫ�����������ʼ��ַ
	Length-Ҫ����������ֽڳ���
	���أ�>=0-�ɹ���<0-ʧ��
	*/
	int (*Erase)(unsigned int StartAddress,unsigned int Length);

	/*****************************************************************************
	* �� �� ���� IsErased
	* �������ܣ� ���ĳһ�������Ƿ���Ҫ����
	* ��    ���� StartAddress-��ʼ��ַ
	* ��    ���� Length-Ҫ�������ĳ��ȣ���λ���ֽ�,���Ϊ0����ִ�м������
	* ��    ���� EraseValue-���ز������ֵ�����Ϊ�գ��򲻷���
	* ��    �أ� int ����Ҫ�������ֽ���
	* ��    ����
	
	* �� �� �ߣ����� 
	* ��    �ڣ� 2022/08/01
	
	* �޸���ʷ��
	** 1. �´���
	*****************************************************************************/
	int (*IsNeedErase)(unsigned int* StartAddress,unsigned int Length,char* EraseValue);

	/*����˵��������ʱ�������е�����д��洢
	����˵����
	���������
	Address-Ҫд��ĵ�ַ
	���أ�>=0-ʵ��д���������<0-�쳣
	*/
	int (*Fulsh)(void);	
    /* other methods */

/*****************************************************************************
* �� �� ���� State
* �������ܣ� ��ȡ�洢����״̬
* ��    ���� Address-�洢����ַ
* ��    �أ�e_FlashState_t���͵�ֵ
* ��    ���� ���ǵ�FLASH���ܷ�Ϊ�ü���������������Ҫָ���洢����ַ

* �� �� �ߣ����� 
* ��    �ڣ� 2023-04-14

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	e_StorageState_t (*State)(unsigned int Address);

/*****************************************************************************
* �� �� ���� DisableEWProtect,EnableEWProtect
* �������ܣ� �رղ�д������ʹ�ܲ�д����
* ��    ���� Address-�洢����ַ
* ��    ���� Size-��������С
* ��    �أ� ����-�쳣�룬�Ǹ���-ʵ�ʹ����Ĵ�С
* ��    ���� 

* �� �� �ߣ����� 
* ��    �ڣ� 2023-04-14

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	bool (*DisableProtect)(unsigned int Address,unsigned int Size,bool RDPFlag);
	bool (*EnableProtect)(unsigned int Address,unsigned int Size);
	bool (*IsEWProtect)(unsigned int Address,unsigned int Size);

end_def_interface(i_Storage_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_Storage_t* const zmStorageManager;

/*============================ PROTOTYPES ====================================*/

#if defined(__Storage_CLASS_IMPLEMENT__) || defined(__Storage_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void Storage_protected_method_example(c_Storage_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __Storage_CLASS_INHERIT__
#undef __Storage_CLASS_IMPLEMENT__

#endif
/* EOF */