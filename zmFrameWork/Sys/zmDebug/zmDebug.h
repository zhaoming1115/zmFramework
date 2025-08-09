/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： zmDebug.h 
开发日期：2025-04-09 
文件功能：定义打印宏。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __ZM_DEBUG_H_
#define __ZM_DEBUG_H_

//#include "PAL/Inc/IPort.h"
#include <stdbool.h>
#include <stdio.h>


//typedef enum
//{
//	Debug_Level_None,				//不输出信息
//	Debug_Level_Error,				//输出异常信息（有具有业务实现）
//	Debug_Level_Main,				//输出重点调试信息
//	Debug_Level_Log,				//输出所有调试信息
//}MJ_DebugLevelLX;

#define	Debug_Level_None	0			//不输出信息
#define	Debug_Level_Error	1			//输出异常信息（有具有业务实现）
#define	Debug_Level_Main	2			//输出重点调试信息
#define	Debug_Level_Log		3				//

#ifdef DebugMode
#define DebugLevel					Debug_Level_Log		//0-不输出调试信息，1-输出异常信息（有具有业务实现），2-输出重点调试信息，3-输出所有调试信息

#else
#define DebugLevel					Debug_Level_Error		//0-不输出调试信息，1-输出异常信息（有具有业务实现），2-输出重点调试信息，3-输出所有调试信息

#endif

#define SPrintf_0(fmt,args...)		printf(fmt,##args)
#define APrintf_0(fmt,args...)		Debug->Printf(fmt,##args)

#if DebugLevel>=1
#define SPrintf_1(fmt,args...)		printf(fmt,##args)
#define APrintf_1(fmt,args...)		Debug->Printf(fmt,##args)
#define SPrintf_DateTime(Level,DT)	SPrintf(Level,"%d-%02d-%02d %02d:%02d:%02d.%03d\n",(DT)->Date.Year,(DT)->Date.Month,(DT)->Date.DayOfMonth, \
															(DT)->Time.Hour,(DT)->Time.Minute,(DT)->Time.Second,(DT)->Time.MillSecond)
#define SPrintf_DateTimeFromMillSecond(Level,MillSeconds)	({JG_DateTimeLX DT; \
																DateTime_FromTotalMillSecond(MillSeconds,&DT); \
																SPrintf_DateTime(Level,&DT);})
#define SPrintf_DateTimeFromSecond(Level,Seconds)				({JG_DateTimeLX DT; \
																DateTime_FromTotalSecond(Seconds,&DT); \
																DT.Time.MillSecond=0; \
																SPrintf_DateTime(Level,&DT);})

#else
#define SPrintf_1(fmt,args...)		
#define APrintf_1(fmt,args...)	
#define SPrintf_DateTime(Level,DT)
#define SPrintf_DateTimeFromMillSecond(Level,MillSeconds)
#define SPrintf_DateTimeFromSecond(Level,Seconds)

#endif

#if DebugLevel>=2
#define SPrintf_2(fmt,args...)		printf(fmt,##args)
#define APrintf_2(fmt,args...)		Debug->Printf(fmt,##args)
#else
#define SPrintf_2(fmt,args...)		0
#define APrintf_2(fmt,args...)	
#endif

#if DebugLevel>=3
#define SPrintf_3(fmt,args...)		printf("来源:[%s] --> [%s] --> 行:[%d]:"fmt,__FILE__,__FUNCTION__,__LINE__,##args)
#define APrintf_3(fmt,args...)		Debug->Print("来源:[%s] --> [%s] --> 行:[%d]:"fmt,__FILE__,__FUNCTION__,__LINE__,##args)
#else
#define SPrintf_3(fmt,args...)		
#define APrintf_3(fmt,args...)		
#endif

#define SPrintf(Level,fmt,args...)  SPrintf_##Level(fmt,##args)		//同步打印输出，使用阻塞方式
#define APrintf(Level,fmt,args...)  APrintf_##Level(fmt,__VA_ARGS__)		//异步打印输出，不能保证能够百分之百打印出来，但对宿主程序影响最小

/*测试某个功能花费的时间*/
#if DebugLevel>=2
#define SPrintfFun_ms(Fun)		unsigned int SafeName(SPrintfFun)=MCU->GetmTick();	\
								Fun; \
								SPrintf(2,"任务 %s 用时：%dms\n",#Fun,MCU->GetmTickSpan(SafeName(SPrintfFun)))

#define SPrintfFun_us(Fun)		({unsigned int SPrintfFun_us_tick=MCU->GetuTick();	\
								(Fun); \
								SPrintf(2,"任务 %s 用时：%dus\n",#Fun,MCU->GetuTickSpan(SPrintfFun_us_tick));})



#else
#define SPrintfFun_ms(Fun)		Fun
#define SPrintfFun_us(Fun)		Fun
#define SPrintf_Row(RowName,ColumnNameTable,RowDataBase,ColumnSpan,DataCount,Format)	
#define SPrintf_Row_User(RowName,ColumnNameTable,RowDataBase,ColumnSpan,DataCount,UserFun)
#define SPrintf_Table(ColumnNameTable,RowNameTable,DataTable,ColumnSpan,RowSpan,Format)
#define SPrintf_Table_User(ColumnNameTable,RowNameTable,DataTable,ColumnSpan,RowSpan,UserFun)
#endif

#define SPrintfData(Level,Data,Length)				for(int ___DataID=0;___DataID<Length;___DataID++){\
														SPrintf(Level,"%02X ",((unsigned char*)Data)[___DataID]);\
													} \
													SPrintf(Level,"\n");


#define SPrintf_Error				SPrintf_0
#define SPrintf_MainInfo			SPrintf_1
#define SPrintf_Info				SPrintf_2
#define SPrintf_AllInfo				SPrintf_3
	
//#define SPrintf(fmt,Valist) 		printf(fmt,Valist)
//#define Printflog(fmt,args...) 		printf("来源:[%s] --> [%s] --> 行:[%d]:"fmt,__FILE__,__FUNCTION__,__LINE__,##args)
//#define	APrintf(xx,arg...)			Debug->Printf(xx,##args)
//#define APrintflog(fmt,args...) 	Debug->Print("来源:[%s] --> [%s] --> 行:[%d]:"fmt,__FILE__,__FUNCTION__,__LINE__,##args)
#endif