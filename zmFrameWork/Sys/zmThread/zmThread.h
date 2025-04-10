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
/*���ļ�����PLOOC������������ԭ��˵��*/
/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� zmThread.h 
�������ڣ�2025-03-28 
�ļ����ܣ���ӦC�ļ���ͷ�ļ�������C�ļ����⹫�����������͡��ӿں�����ȫ�ֱ�����

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/


#ifndef __zmThread_H__
#define __zmThread_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "zmThread_Simple.h"
#include "../zmList/zmList.h"

/*============================ MACROS ========================================*/
#define Thread_UseGaoJingDuTick				0    //����ʹ�ø߾���ʱ��

#if Thread_UseGaoJingDuTick
#define Thread_TickSizeUse64bit				1
#define Thread_WaitSignalSleepTime			1000
#else
#define Thread_WaitSignalSleepTime			1

#endif						
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
#if Thread_TickSizeUse64bit
typedef long long threadtick_t;
typedef long long threadtickspan_t;

#else
typedef unsigned int threadtick_t;
typedef int threadtickspan_t;

#endif

typedef int (*f_ThreadProcess)(void);

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__Thread_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__Thread_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class Thread_t
//! @{
declare_class(c_Thread_t)

def_class(c_Thread_t,
	which(implement(s_ProtoThread_t))
    public_member(
        //!< place your public members here
    )

    private_member(
        //!< place your private members here
		threadtick_t NextCallTick;	//�߳��´ε��õ�ʱ��
		f_ThreadProcess Porcess;		//�߳�����
		unsigned char ThreadID;		//�߳�ID
		unsigned char ThreadState;	//�߳�״̬
		unsigned char OtherCallFlag;	//�����ȼ����ñ�־
		unsigned char ThreadYouXianJi;	//�߳����ȼ�
    )
    
    protected_member(
        //!< place your private members here
		implement(c_ListItem_t)
    )
)

end_def_class(c_Thread_t) /* do not remove this for forward compatibility  */
//! @}

typedef enum
{
	Thread_YXJ_CallByLunXun,
	Thread_YXJ_CallByTimer,
	Thread_YXJ_Nums,
}e_ThreadYouXianJi_t;

typedef struct Thread_cfg_t {
    
    //! put your configuration members here
	unsigned char MaxThreadCount;		//���֧�ֵ��߳��������̷߳���������������������̳߳ش�С������Խ��ռ�õ��ڴ�ҲԽ��
	unsigned char TimerTickUnitLevel;		//�ж϶�ʱ��ʱ����λ�ȼ�����2��ʱ����λTimerTickUnit= zmSys->GetTick() >> TimerTickUnitLevel
	unsigned char TimerIntMinDelayUnit;	//������ʱ�������ж�ʱ��ʱ����Ҫ��ʱ��TimerTickUnit����
	unsigned char REV;		//����ʹ��
    
} s_Thread_cfg_t;


//! \name interface i_Thread_t
//! @{
def_interface(i_Thread_t)
    int (*Init)(const s_Thread_cfg_t *ptCFG,unsigned int SysTickPrescalerLevel);
	/*****************************************************************************
	* �� �� ���� CreateAddThread
	* �������ܣ� ��鲢�����߳�
	* ��    ���� Fun-�̵߳�����
				YouXianJi-���ȼ�
				CHeckSame-����Ϊtrueʱ�����������ȼ���Ƿ��Ѿ�������ָ��������̡߳����У��򷵻��Ǹ��̣߳����򴴽����߳�
				
	* ��    ����
	
	* �� �� �ߣ����� 
	* ��    �ڣ� 2022/11/29
	
	* �޸���ʷ��
	** 1. �´���
	*****************************************************************************/
	c_Thread_t* (*CreateAddThread)(f_ThreadProcess Process,e_ThreadYouXianJi_t YouXianJi,bool CHeckSame);

/*
ͨ���̺߳���ɾ���߳�
Fun():�����߳�ʱ����ĺ�����ģ�����£�
int FunName(JG_ThreadLX* zmThread)
{
/�ֲ�������

/

	ZT_BEGIN(zmThread);



	ZT_END(zmThread);


}
*/
	/*****************************************************************************
	* �� �� ���� RemoveThreadWithFun
	* �������ܣ� �Ƴ�ָ��ָ������������߳�
	* ��    ���� Fun-����ָ��
	* ��    �أ� ��
	* ��    ���� 
	
	* �� �� �ߣ����� 
	* ��    �ڣ� 2022/11/29
	
	* �޸���ʷ��
	** 1. �´���
	*****************************************************************************/
	void (*RemoveThreadWithProcess)(f_ThreadProcess Process);
/*
���к���
ÿ����һ�Σ�������ڲ����߳��б���ѯһ�飬��ִ�з�����ѯ�����������������
������Ӧ�ý��˺������� WHILE(1){}ѭ������
*/
	void (*Run)(void);
	/*****************************************************************************
	* �� �� ���� RemoveThread
	* �������ܣ��Ƴ�ָ�����߳�
	* ��    ���� Thread-�߳�ָ��,��Ϊ�գ�����ɾ����ǰ�߳�
	* ��    �أ� ��
	* ��    ����
	
	* �� �� �ߣ����� 
	* ��    �ڣ� 2022/11/29
	
	* �޸���ʷ��
	** 1. �´���
	*****************************************************************************/
	void (*RemoveThread)(c_Thread_t* Thread);

/*****************************************************************************
* �� �� ���� Sleep
* �������ܣ� ��ָ�����߳�����һ��ʱ��
* ��    ���� Thread: Ҫ���ߵ��߳�ָ�� 
**			 SleepTime:Ҫ���ߵ�ʱ�������ڵ��ȷ�ʽΪ��ѯ���̣߳���λΪms���������̵߳�λΪus
			WithCallTickFlag: true-���жϵ��õ�ʱ��Ϊ��׼��false-�Ե�ǰʱ��Ϊ��׼
* ��    �أ� ��
* ��    ���� ���ڵ��ȷ�ʽΪ��ѯ���̣߳�ִ�иú����󣬽���ӵ�ǰʱ�俪ʼ������SleepTimeָ���ĺ�������
			���ڵ��ȷ�ʽΪTimer���̣߳�ִ�иú����󣬻���ʱSleepTimeָ����΢�������ٴ������̵߳��жϣ�


* �� �� �ߣ����� 
* ��    �ڣ� 2024-02-01

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	void (*Sleep)(unsigned int SleepTime,bool WithCallTickFlag);

end_def_interface(i_Thread_t) /*do not remove this for forward compatibility */
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_Thread_t* const zmThreadManager;

#define ZT_Sleep2(__SleepTime,__WithCallTickFlag) 		zmThreadManager->Sleep(__SleepTime,__WithCallTickFlag); \
														ZT_YIELD_SLEEP()
#define ZT_Sleep1(__SleepTime) 							ZT_Sleep2(__SleepTime,false)

#define ZT_Sleep(...)  									SYS_EVAL(ZT_Sleep, __VA_ARGS__)(__VA_ARGS__)

#define ZT_SubSleep(__Thread,__SleepTime) 				zmThreadManager->Sleep(__SleepTime,false); \
														ZT_YIELD_SLEEP(__Thread)

#define ZT_WaitSignal2(__Thread,__Signal) 				while(!(__Signal)){ \
															ZT_SubSleep(__Thread,1);}
#define ZT_WaitSignal1(__Signal) 						while(!(__Signal)){ \
															ZT_Sleep(Thread_WaitSignalSleepTime);}
#define ZT_WaitSignal(...)  							SYS_EVAL(ZT_WaitSignal, __VA_ARGS__)(__VA_ARGS__)

/*============================ PROTOTYPES ====================================*/

#if defined(__Thread_CLASS_IMPLEMENT__) || defined(__Thread_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void Thread_protected_method_example(c_Thread_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __Thread_CLASS_INHERIT__
#undef __Thread_CLASS_IMPLEMENT__

#endif
/* EOF */