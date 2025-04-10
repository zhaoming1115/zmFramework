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
/*本文件基于PLOOC开发，以上是原版说明*/
/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： zmDateTime.h 
开发日期：2024-04-06 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

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
#define __UH_DATE_HourToUK						8		//时区，东为正，西为负
#define	__UH_DATE_BaseYear						1970
#define __UH_DATE_BaseWeekDay					4
//#define __UH_DATE_DayCountToUK					18993
#endif
#define DateTime_Format						"##时间格式：yyyy-MM-dd HH:mm:ss.zzz"
#define DateTime_Printf(...)		printf(__VA_ARGS__)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//时间类型
typedef enum
{
	DT_Valid_None,
	DT_Valid_DefaultValue,	//时间是通过默认值校准的
	DT_Valid_UserValue,	//时间是通过指定值校准的
}e_DateTimeValid_t;

typedef union
{
	unsigned int Value;
	struct
	{
		unsigned int TimeIsValid:2;	//0-时间无效，1-时间是有效，但是不一定准确，2-时间是准确的
		unsigned int SecondPWMIsOK:1;
		unsigned int TimeFormat:1;	//时间格式：0-24小时，1-12小时，目前仅支持24小时制
		unsigned int BaoLiu:1;
		unsigned int MillSecond:10;
		unsigned int Second:6;
		unsigned int Minute:6;
		unsigned int Hour:5;
	};
}u_Time_t;

typedef u_Time_t				s_Time_t;
//日期类型
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

//日期时间类型
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
	int (*SetDateTime)(__In s_DateTime_t* NewDateTime); //设置万年历时间，16进制格式

/*****************************************************************************
* 函 数 名： TotalSecond，TotalMillSecond
* 函数功能： 计算给定日期时间相对于基准时间的总秒数、总毫秒数
* 参    数： DT: 给定的时间对象，若为空，则取当前时间
* 返    回： 总秒数
* 描    述：

* 开 发 者：赵明 
* 日    期： 2024-04-13

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	unsigned int (*ToTotalSecond)(__In s_DateTime_t* DT);
	long long (*ToTotalMillSecond)(__In s_DateTime_t* DT);

/*****************************************************************************
* 函 数 名： FromTotalSecond,FromTotalMillSecond
* 函数功能： 通过相对于基准时间的总秒数或者总毫秒数获取时间
* 参    数： TotalSecond【TotalMillSecond】: 相对于基准时间的总秒数【总毫秒数】，若为0，代表使用当前时间
**			 DT: [输出] ：要输出的时间对象
* 返    回： 若成功，则返回0，异常返回负数
* 描    述：

* 开 发 者：赵明 
* 日    期： 2024-04-13

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	int (*FromTotalSecond)(unsigned int TotalSecond,__Out s_DateTime_t* DT);
	int (*FromTotalMillSecond)(long long TotalMillSecond, __Out s_DateTime_t* DT);
	
/*****************************************************************************
* 函 数 名： IsVaid
* 函数功能： 检查日期时间是否是有效的
* 参    数： DT: 要检查的日期时间对象，若为空，则返回当前时间是否已经被用户校准过
* 返    回：true-有效的，false-无效的
* 描    述：

* 开 发 者：赵明 
* 日    期： 2024-08-12

* 修改历史：
** 1. 新创建

 *****************************************************************************/
	bool (*IsVaid)(__In s_DateTime_t* DT);
/*****************************************************************************
* 函 数 名： int
* 函数功能：
* 参    数： DateTime: 若为空，则取当前时间 
* 参    数： DateTime: 若为空，则取当前时间 
* 返    回：
* 描    述：

* 开 发 者：赵明 
* 日    期： 2024-04-06

* 修改历史：
** 1. 新创建

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
