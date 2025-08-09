/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� zmDebug.h 
�������ڣ�2025-04-09 
�ļ����ܣ������ӡ�ꡣ

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/

 #ifndef __ZM_DEBUG_H_
#define __ZM_DEBUG_H_

//#include "PAL/Inc/IPort.h"
#include <stdbool.h>
#include <stdio.h>


//typedef enum
//{
//	Debug_Level_None,				//�������Ϣ
//	Debug_Level_Error,				//����쳣��Ϣ���о���ҵ��ʵ�֣�
//	Debug_Level_Main,				//����ص������Ϣ
//	Debug_Level_Log,				//������е�����Ϣ
//}MJ_DebugLevelLX;

#define	Debug_Level_None	0			//�������Ϣ
#define	Debug_Level_Error	1			//����쳣��Ϣ���о���ҵ��ʵ�֣�
#define	Debug_Level_Main	2			//����ص������Ϣ
#define	Debug_Level_Log		3				//

#ifdef DebugMode
#define DebugLevel					Debug_Level_Log		//0-�����������Ϣ��1-����쳣��Ϣ���о���ҵ��ʵ�֣���2-����ص������Ϣ��3-������е�����Ϣ

#else
#define DebugLevel					Debug_Level_Error		//0-�����������Ϣ��1-����쳣��Ϣ���о���ҵ��ʵ�֣���2-����ص������Ϣ��3-������е�����Ϣ

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
#define SPrintf_3(fmt,args...)		printf("��Դ:[%s] --> [%s] --> ��:[%d]:"fmt,__FILE__,__FUNCTION__,__LINE__,##args)
#define APrintf_3(fmt,args...)		Debug->Print("��Դ:[%s] --> [%s] --> ��:[%d]:"fmt,__FILE__,__FUNCTION__,__LINE__,##args)
#else
#define SPrintf_3(fmt,args...)		
#define APrintf_3(fmt,args...)		
#endif

#define SPrintf(Level,fmt,args...)  SPrintf_##Level(fmt,##args)		//ͬ����ӡ�����ʹ��������ʽ
#define APrintf(Level,fmt,args...)  APrintf_##Level(fmt,__VA_ARGS__)		//�첽��ӡ��������ܱ�֤�ܹ��ٷ�֮�ٴ�ӡ������������������Ӱ����С

/*����ĳ�����ܻ��ѵ�ʱ��*/
#if DebugLevel>=2
#define SPrintfFun_ms(Fun)		unsigned int SafeName(SPrintfFun)=MCU->GetmTick();	\
								Fun; \
								SPrintf(2,"���� %s ��ʱ��%dms\n",#Fun,MCU->GetmTickSpan(SafeName(SPrintfFun)))

#define SPrintfFun_us(Fun)		({unsigned int SPrintfFun_us_tick=MCU->GetuTick();	\
								(Fun); \
								SPrintf(2,"���� %s ��ʱ��%dus\n",#Fun,MCU->GetuTickSpan(SPrintfFun_us_tick));})



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
//#define Printflog(fmt,args...) 		printf("��Դ:[%s] --> [%s] --> ��:[%d]:"fmt,__FILE__,__FUNCTION__,__LINE__,##args)
//#define	APrintf(xx,arg...)			Debug->Printf(xx,##args)
//#define APrintflog(fmt,args...) 	Debug->Print("��Դ:[%s] --> [%s] --> ��:[%d]:"fmt,__FILE__,__FUNCTION__,__LINE__,##args)
#endif