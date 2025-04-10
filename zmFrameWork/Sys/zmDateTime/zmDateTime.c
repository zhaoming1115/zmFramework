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
�� �� ���� zmDateTime.c 
�������ڣ�2025-03-28 
�ļ����ܣ�ʵ������ʱ�����

��    ����

�����ӿڣ�

�ṩ�ӿڣ�zmDateTime����
	

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/


/*============================ INCLUDES ======================================*/
#define __DateTime_CLASS_IMPLEMENT__

#include "zmDateTime.h"
#include "../zmSys/zmSys.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "stdio.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)    assert(__VA_ARGS__)
#endif

#undef this
#define this        (ptThis)

#define DateTime_mTick()		zmSys->GetmTick()

/*============================ TYPES =========================================*/
typedef struct
{
	unsigned char DateTimeIsValid;	//ʱ����Ч��־���˱�־�ϵ�����ʱ���㣬�ڵ���SetDateTime��������1
}s_DateTimeData_t;

/*============================ LOCAL VARIABLES ===============================*/
static const char mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static long long g_BaseTick __NoInitRAM;
static s_DateTimeData_t g_RunParm={0};

/*============================ PROTOTYPES ====================================*/

/*============================ ˽�к��� =========================================*/
static int DateTime_IsLeapYear(unsigned int year)
{
	if(year % 4 == 0)
	{
		if(year % 100 == 0)
		{
			if(year % 400 == 0)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 0;
	}
}

/*****************************************************************************
* �� �� ���� DateTime_ConvertToTotalDays
* �������ܣ� ��ָ�������ڶ���ת��Ϊ���������
* ��    ���� Date: ָ�������ڶ��� 
* ��    �أ� ���������
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2023-08-10

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
static unsigned int DateTime_ConvertToTotalDays(__In s_Date_t* Date)
{
	unsigned int DayCounter=0;
	
	for(unsigned int t = (unsigned int)G_BaseDateTime->Date.Year; t < (unsigned int)Date->Year; t++)
	{
		if(DateTime_IsLeapYear(t))
		{
			DayCounter += 366;
		}
		else
		{
			DayCounter += 365;
		}
	}

	unsigned char tmp=Date->Month-1;

	for(unsigned int t = 0; t < tmp; t++)
	{
		DayCounter += mon_table[t];
		if(DateTime_IsLeapYear((unsigned int)Date->Year) && t == 1)	//����������2��
		{
			DayCounter++;
		}
	}

	DayCounter += Date->DayOfMonth-1;
	return DayCounter;
}

/*****************************************************************************
* �� �� ���� DateTime_FromTotalDays
* �������ܣ� �����������ת��Ϊ����
* ��    ���� TotalDays:��������� 
**			 To: Ҫת���������ڶ��� 
* ��    �أ� 0
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2023-08-10

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
static int DateTime_FromTotalDays(unsigned int TotalDays,s_Date_t* To)
{	
	To->DayOfWeek=(G_BaseDateTime->Date.DayOfWeek+TotalDays)%7;
	unsigned int Year=(unsigned int)G_BaseDateTime->Date.Year;
	int Days=(int)TotalDays;
	while(Days >= 365)
	{
		if(DateTime_IsLeapYear(Year))
		{
			if(Days >= 366)
			{
				Days -= 366;
			}
			else
			{
				Year++;
				break;
			}
		}
		else
		{
			Days -= 365;
		}

		Year++;
	}
	To->Year = (short)Year;

	unsigned char Month = 0;
	while(Days >= 28)
	{
		if(DateTime_IsLeapYear((unsigned int)To->Year) && Month == 1)
		{
			if(Days >= 29)
			{
				Days -= 29;
			}
			else
			{
				break;
			}
		}
		else
		{
			if(Days >= mon_table[Month])
			{
				Days -= mon_table[Month];
			}
			else
			{
				break;
			}
		}

		Month++;
	}

	To->Month = Month + 1;
	To->DayOfMonth = (unsigned char)Days + 1;
	return 0;
	
}

/*****************************************************************************
* �� �� ���� DateTime_IsVaidDate
* �������ܣ� ���ָ�������ڶ����Ƿ�Ϸ�
* ��    ���� Date: ָ�������ڶ��� 
* ��    �أ�true-�Ϸ���flase-���Ϸ�
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2023-08-10

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
static bool DateTime_IsVaidDate(const s_Date_t* Date)
{
	if (Date->Year>=G_BaseDateTime->Date.Year && Date->Year<2100 && Date->Month>0 && Date->Month<13)
	{
		int daycount=mon_table[Date->Month-1];
		if (Date->Month==2 && DateTime_IsLeapYear((unsigned int)Date->Year))
		{
			daycount++;
		}
		if (Date->DayOfMonth>0 && Date->DayOfMonth<=daycount)
		{
			return true;
		}
	}
	return false;
}

/*****************************************************************************
* �� �� ���� DateTime_IsVaidTime
* �������ܣ� ���ָ����ʱ������Ƿ�Ϸ�
* ��    ���� Time: ָ����ʱ����� 
* ��    �أ�true-�Ϸ���flase-���Ϸ�
* ��    ����

* �� �� �ߣ����� 
* ��    �ڣ� 2023-08-10

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
static bool DateTime_IsVaidTime(const s_Time_t* Time)
{
	return Time->Hour<24 && Time->Minute<60 && Time->Second<60 && Time->MillSecond<1000;
}

static bool DateTime_IsVaid(const 	s_DateTime_t* DateTime)
{
	return (DateTime==NULL)?(g_RunParm.DateTimeIsValid==DT_Valid_UserValue):
			(DateTime_IsVaidDate(&DateTime->Date))?DateTime_IsVaidTime(&DateTime->Time):false;
}

inline static long long DateTime_Now(void)
{
	long long mTick=DateTime_mTick();
	mTick+=g_BaseTick;
	return mTick;
}

/*============================ IMPLEMENTATION ================================*/
__WEAK const s_DateTime_t g_DefaultDateTime={.Time={.Hour=Bulid_HOUR,.Minute=Bulid_MINUTE,.Second=0,.MillSecond=0},.Date={.Year=Bulid_YEAR,.Month=Bulid_MONTH,.DayOfMonth=Bulid_DAY}};

static
unsigned int DateTime_ToTotalSecond(__In s_DateTime_t* DateTime)
{
	if(DateTime)
	{
		unsigned int DayCounter=DateTime_ConvertToTotalDays(&DateTime->Date);
		unsigned int secCounter=DayCounter*24+DateTime->Time.Hour;	
		secCounter=secCounter*60+DateTime->Time.Minute;
		secCounter=secCounter*60+DateTime->Time.Second;
		return secCounter;
	}
	else
	{
		long long mTick=DateTime_Now();
		mTick/=1000;
		return (unsigned int)mTick; 		
	}
}

static
long long DateTime_ToTotalMillSecond(__In s_DateTime_t* DateTime)
{
	long long mTick;
	if(DateTime)
	{
		unsigned int secCounter=DateTime_ToTotalSecond(DateTime);	
		mTick=(long long)secCounter*1000+DateTime->Time.MillSecond;
	}
	else
	{
		mTick=DateTime_Now();
	}
	return mTick;
}

static
int DateTime_FromTotalSecond(unsigned int TotalSecond,__Out s_DateTime_t* To)
{
	unsigned int tmp=TotalSecond;
	To->Time.Second=tmp%60;
	tmp/=60;
	To->Time.Minute=tmp%60;
	tmp/=60;
	To->Time.Hour=tmp%24;
	tmp/=24;
	return DateTime_FromTotalDays(tmp,&To->Date);
}

static
int DateTime_FromTotalMillSecond(long long TotalMillSecond, __Out s_DateTime_t* To)
{
	lldiv_t tmp64;
	tmp64=lldiv(TotalMillSecond,1000);
	To->Time.MillSecond=(unsigned int)tmp64.rem;
	return DateTime_FromTotalSecond((unsigned int)tmp64.quot,To);
}

static
int DateTime_GetDateTime(__Out s_DateTime_t* To,int Addms)
{
	To->Time.TimeIsValid=(g_RunParm.DateTimeIsValid==DT_Valid_UserValue);
	return DateTime_FromTotalMillSecond(DateTime_Now()+Addms,To);	
}

static
int DateTime_SetDateTime(__In s_DateTime_t* NewDateTime)
{
	if(NewDateTime==NULL)
	{
		NewDateTime=G_DefaultDateTime;
	}
	else
	{
		if(!DateTime_IsVaid(NewDateTime))
		{
			return Err_Code_ParmFail;
		}		
	}
	long long mTick=DateTime_ToTotalMillSecond(NewDateTime);
	long long NowTick=DateTime_mTick();
	g_BaseTick=mTick-NowTick;
	if(NewDateTime==G_DefaultDateTime)
	{
	}
	g_RunParm.DateTimeIsValid=(NewDateTime==G_DefaultDateTime)?DT_Valid_DefaultValue:DT_Valid_UserValue;
	return 0;
}


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
static
const char* DateTime_ToString(__In s_DateTime_t* DateTime,__Out char* Buffer)
{
	s_DateTime_t _DT;
	if(DateTime==NULL)
	{
		DateTime_GetDateTime(&_DT,0);
		DateTime=&_DT;
	}
	sprintf(Buffer,"%d-%d-%d %d:%d:%d.%d",DateTime->Date.Year,DateTime->Date.Month,DateTime->Date.DayOfMonth,
										DateTime->Time.Hour,DateTime->Time.Minute,DateTime->Time.Second,DateTime->Time.MillSecond);
	return Buffer;
}

static
int DateTime_ParseString(__In char* Buffer,__Out s_DateTime_t* DateTime)
{
	s_DateTime_t _DT;
	int tmp[7]={0};
	int rst=sscanf(Buffer,"%04d-%02d-%02d %02d:%02d:%02d.%03d",&tmp[0],&tmp[1],&tmp[2],&tmp[3],&tmp[4],&tmp[5],&tmp[6]);
	if(rst<5) return Err_Code_ParmFail;

	{
		bool SetSysTimeFlag=false;
		if(DateTime==NULL)
		{
			DateTime=&_DT;
			SetSysTimeFlag=true;
		}
		DateTime->Date.Year=tmp[0];
		DateTime->Date.Month=tmp[1];
		DateTime->Date.Day=tmp[2];
		DateTime->Time.Hour=tmp[3];
		DateTime->Time.Minute=tmp[4];
		DateTime->Time.Second=tmp[5];
		DateTime->Time.Second=tmp[6];
		if(SetSysTimeFlag) return DateTime_SetDateTime(DateTime);
	}

	return 0;
}

/*============================ GLOBAL VARIABLES ==============================*/
const i_DateTime_t* const zmDateTime = &(const i_DateTime_t){
	.ToTotalSecond=DateTime_ToTotalSecond,
	.ToTotalMillSecond=DateTime_ToTotalMillSecond,
	.FromTotalSecond=DateTime_FromTotalSecond,
	.FromTotalMillSecond=DateTime_FromTotalMillSecond,
	.IsVaid=DateTime_IsVaid,
	.GetDateTime=DateTime_GetDateTime,
	.SetDateTime=DateTime_SetDateTime,
	.ToString=DateTime_ToString,
    .ParseString=DateTime_ParseString,
    /* other methods */
};

/* EOF */