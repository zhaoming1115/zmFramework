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
�� �� ���� zmDateTime.h 
�������ڣ�2024-04-06 
�ļ����ܣ���ӦC�ļ���ͷ�ļ�������C�ļ����⹫�����������͡��ӿں�����ȫ�ֱ�����

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/


#ifndef __DateTime_H__
#define __DateTime_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../Sys_cfg.h"
#include "../Sys_DataType.h"

/*============================ MACROS ========================================*/
#ifndef __UH_DATE_BaseYear
#define __UH_DATE_HourToUK						8		//ʱ������Ϊ������Ϊ��
#define	__UH_DATE_BaseYear						1970
#define __UH_DATE_BaseWeekDay					4
//#define __UH_DATE_DayCountToUK					18993
#endif
#define DateTime_Format						"##ʱ���ʽ��yyyy-MM-dd HH:mm:ss.zzz"
#define DateTime_Printf(...)		printf(__VA_ARGS__)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//ʱ������
typedef enum
{
	DT_Valid_None,
	DT_Valid_DefaultValue,	//ʱ����ͨ��Ĭ��ֵУ׼��
	DT_Valid_UserValue,	//ʱ����ͨ��ָ��ֵУ׼��
}e_DateTimeValid_t;

typedef union
{
	unsigned int Value;
	struct
	{
		unsigned int TimeIsValid:2;	//0-ʱ����Ч��1-ʱ������Ч�����ǲ�һ��׼ȷ��2-ʱ����׼ȷ��
		unsigned int SecondPWMIsOK:1;
		unsigned int TimeFormat:1;	//ʱ���ʽ��0-24Сʱ��1-12Сʱ��Ŀǰ��֧��24Сʱ��
		unsigned int BaoLiu:1;
		unsigned int MillSecond:10;
		unsigned int Second:6;
		unsigned int Minute:6;
		unsigned int Hour:5;
	};
}u_Time_t;

typedef u_Time_t				s_Time_t;
//��������
typedef struct
{
	union
	{
		unsigned char Day;		
		struct
		{
			unsigned char DayOfMonth:5;
			unsigned char DayOfWeek:3;

		};
	};
	unsigned char Month;
	short Year;			
}s_Date_t;

//����ʱ������
typedef struct
{
	s_Time_t Time;
	s_Date_t Date;		//
}s_DateTime_t;
#ifndef G_BaseDateTime
#if __UH_DATE_HourToUK>=0
static const s_DateTime_t g_BaseDateTime={.Time={.Hour=__UH_DATE_HourToUK,.Minute=0,.Second=0,.MillSecond=0},.Date={.Year=__UH_DATE_BaseYear,.Month=0,.DayOfMonth=0,.DayOfWeek=__UH_DATE_BaseWeekDay}};
#else
static const JG_DateTimeLX g_BaseDateTime={.Time={.Hour=24+__UH_DATE_HourToUK,.Minute=0,.Second=0,.MillSecond=0},.Date={.Year=__UH_DATE_BaseYear-1,.Month=12,.DayOfMonth=31,.DayOfWeek=(__UH_DATE_BaseWeekDay+6)%7}};

#endif
extern const s_DateTime_t g_DefaultDateTime;

#define G_BaseDateTime		(&g_BaseDateTime)	
#define G_DefaultDateTime	(&g_DefaultDateTime)	
#endif

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__DateTime_CLASS_IMPLEMENT__)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__DateTime_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif   

#include "plooc_class.h"


//! \name class DateTime_t
//! @{
//! @}
declare_class(c_DateTime_t)

def_class(c_DateTime_t,

    public_member(
        //!< place your public members here
//		implement(i_DateTime_t)
    )

    private_member(
        //!< place your private members here
    )
    
    protected_member(
        //!< place your private members here
    )
)

end_def_class(c_DateTime_t) /* do not remove this for forward compatibility  */


//! \name interface i_DateTime_t
//! @{
def_interface(i_DateTime_t)
 	int (*GetDateTime)(s_DateTime_t* GetTo,int Addms);
	int (*SetDateTime)(__In s_DateTime_t* NewDateTime); //����������ʱ�䣬16���Ƹ�ʽ

/*****************************************************************************
* �� �� ���� TotalSecond��TotalMillSecond
* �������ܣ� �����������ʱ������ڻ�׼ʱ������������ܺ�����
* ��    ���� DT: ������ʱ�������Ϊ�գ���ȡ��ǰʱ��
* ��    �أ� ������
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2024-04-13

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	unsigned int (*ToTotalSecond)(__In s_DateTime_t* DT);
	long long (*ToTotalMillSecond)(__In s_DateTime_t* DT);

/*****************************************************************************
* �� �� ���� FromTotalSecond,FromTotalMillSecond
* �������ܣ� ͨ������ڻ�׼ʱ��������������ܺ�������ȡʱ��
* ��    ���� TotalSecond��TotalMillSecond��: ����ڻ�׼ʱ������������ܺ�����������Ϊ0������ʹ�õ�ǰʱ��
**			 DT: [���] ��Ҫ�����ʱ�����
* ��    �أ� ���ɹ����򷵻�0���쳣���ظ���
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2024-04-13

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	int (*FromTotalSecond)(unsigned int TotalSecond,__Out s_DateTime_t* DT);
	int (*FromTotalMillSecond)(long long TotalMillSecond, __Out s_DateTime_t* DT);
	
/*****************************************************************************
* �� �� ���� IsVaid
* �������ܣ� �������ʱ���Ƿ�����Ч��
* ��    ���� DT: Ҫ��������ʱ�������Ϊ�գ��򷵻ص�ǰʱ���Ƿ��Ѿ����û�У׼��
* ��    �أ�true-��Ч�ģ�false-��Ч��
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2024-08-12

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	bool (*IsVaid)(__In s_DateTime_t* DT);
/*****************************************************************************
* �� �� ���� int
* �������ܣ�
* ��    ���� DateTime: ��Ϊ�գ���ȡ��ǰʱ�� 
* ��    ���� DateTime: ��Ϊ�գ���ȡ��ǰʱ�� 
* ��    �أ�
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2024-04-06

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
	const char* (*ToString)(__In s_DateTime_t* DateTime,__Out char* Buffer);
    /* other methods */
	int (*ParseString)(__In char* Buffer,__Out s_DateTime_t* DateTime);
end_def_interface(i_DateTime_t) /*do not remove this for forward compatibility */
//! @}


/*============================ GLOBAL VARIABLES ==============================*/
extern const i_DateTime_t* const zmDateTime;

/*============================ PROTOTYPES ====================================*/

#if defined(__DateTime_CLASS_IMPLEMENT__) || defined(__DateTime_CLASS_INHERIT__)
/*! \brief a method only visible for current class and derived class */
extern void DateTime_protected_method_example(c_DateTime_t *ptObj);
#endif

/*! \note it is very important to undef those macros */
#undef __DateTime_CLASS_INHERIT__
#undef __DateTime_CLASS_IMPLEMENT__

#endif
/* EOF */
