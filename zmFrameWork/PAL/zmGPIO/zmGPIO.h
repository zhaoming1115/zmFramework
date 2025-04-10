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
�� �� ���� zmGPIO.h 
�������ڣ�2025-03-26 
�ļ����ܣ�����GPIO������ص��ࡣ

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/


#ifndef __GPIO_H__
#define __GPIO_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../PAL_DataType.h"
#include "../PAL_Config.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__GPIO_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__GPIO_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class GPIO_t
//! @{
declare_class(c_GPIO_t)

def_class(c_GPIO_t,

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

end_def_class(c_GPIO_t) /* do not remove this for forward compatibility  */
//! @}


typedef void (*f_GPIOInputCHanged)(void);

//! \name interface i_GPIO_t
//! @{
def_interface(i_GPIO_t)
	int (*Init)(unsigned char SurportExITCount);
	int (*SetMode)(gpio_t GPIOx,e_GPIO_Mode_t mode);
	e_GPIO_Value_t (*ReadPin)(gpio_t GPIOx);
	int (*WritePin)(gpio_t GPIOx,e_GPIO_Value_t GPIO_Value_CHange);

/*****************************************************************************
* �� �� ���� SetExitIT
* �������ܣ������ⲿ�ж�
* ��    ���� GPIOx: �ⲿ�ж϶�Ӧ��GPIO��� 
**			 TriggerMode: ����ģʽ 
**			 CallBack: �����жϺ�Ļص���������Ϊ�գ������ͨ�õ��жϴ����� 
* ��    �أ� 0-�ɹ�����0-�쳣��
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2025-03-26

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	int (*SetExitIT)(gpio_t GPIOx,e_Exit_TriggerMode_t TriggerMode,f_GPIOInputCHanged CallBack);
/*****************************************************************************
* �� �� ���� ClearExitIT
* �������ܣ�����ⲿ�ж�
* ��    ���� GPIOx: �ⲿ�ж϶�Ӧ��GPIO��� 
* ��    �أ� 0-�ɹ�����0-�쳣��
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2025-03-26

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	int (*ClearExitIT)(gpio_t GPIOx);
    /* other methods */

end_def_interface(i_GPIO_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_GPIO_t* const zmGPIOManager;

/*============================ PROTOTYPES ====================================*/

#if defined(__GPIO_CLASS_IMPLEMENT__) || defined(__GPIO_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void GPIO_protected_method_example(c_GPIO_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __GPIO_CLASS_INHERIT__
#undef __GPIO_CLASS_IMPLEMENT__

#endif
/* EOF */
