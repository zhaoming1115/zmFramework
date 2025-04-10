/*****************************************************************************
��Э�̿�ı��� Adam Dunkels ��Protothreads��
���ο� http://blog.chinaaet.com/fy_zhu/p/31842 �еķ�ʽ��������˼�

----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� zmThread_Simple.h 
�������ڣ�2024-03-31 
�ļ����ܣ���ӦC�ļ���ͷ�ļ�������C�ļ����⹫�����������͡��ӿں�����ȫ�ֱ�����

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/
#ifndef __zmThreadSimple_H__
#define __zmThreadSimple_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../PT_Thread/lc.h"
#include "../Sys_cfg.h"

/*============================ MACROS ========================================*/
#define ZT_EXITED		 -1		//�м��˳�
#define ZT_ENDED		0		//�������
#define ZT_TimeOuted	-2	//����ʱ�˳�
#define ZT_YIELDED		-3		//����ת�ÿ���Ȩ

#define ZT_WAITING		-4		//�ȴ��У�С�ڵ��ڴ�ֵʱ�����������ڵȴ���
#define ZT_SLEEPED		-5

/*============================ �꺯���������߳� ===========================*/
typedef struct  {
  lc_t lc;
//  unsigned int TimeOutms;
}s_ProtoThread_t;

extern s_ProtoThread_t* G_CurThread;

#define ZT_NEW1(Name) 	static s_ProtoThread_t SAFE_NAME(Name)={0};\
						s_ProtoThread_t* Name=&SAFE_NAME(Name)

#define ZT_NEW0() 		ZT_NEW1(SAFE_NAME(Thread))
#define ZT_NEW(...)   	SYS_EVAL(ZT_NEW, __VA_ARGS__)(__VA_ARGS__)
					

#define ZT_INIT1(pt) 	LC_INIT((pt)->lc)
#define	ZT_INIT0()		ZT_INIT1(G_CurThread)
#define ZT_INIT(...)    SYS_EVAL(ZT_INIT, __VA_ARGS__)(__VA_ARGS__)

#define ZT_BEGIN1(pt) 	{ LC_RESUME((pt)->lc)
#define	ZT_BEGIN0()		ZT_BEGIN1(G_CurThread)
#define ZT_BEGIN(...)    SYS_EVAL(ZT_BEGIN, __VA_ARGS__)(__VA_ARGS__)

#define ZT_END2(pt, ReturnCode)		 LC_END((pt)->lc); \
									ZT_INIT(pt); \
									return ReturnCode; }
#define ZT_END1(pt)		ZT_END2(pt,ZT_ENDED)
#define	ZT_END0()		ZT_END1(G_CurThread)
#define ZT_END(...)    SYS_EVAL(ZT_END, __VA_ARGS__)(__VA_ARGS__)

#define ZT_THREAD(name_args) int name_args

#define ZT_WAIT_UNTIL3(pt, condition,ReturnCode) 	do { \
														while(!(condition))	{ \
														LC_YIELD((pt)->lc,ReturnCode); \
														} \
													} while(0)
#define ZT_WAIT_UNTIL2(pt, condition) 	ZT_WAIT_UNTIL3(pt, condition,ZT_WAITING)
#define	ZT_WAIT_UNTIL1(condition)		ZT_WAIT_UNTIL2(G_CurThread,condition)
#define ZT_WAIT_UNTIL(...)    			SYS_EVAL(ZT_WAIT_UNTIL, __VA_ARGS__)(__VA_ARGS__)

#define ZT_YIELD2(pt,ReturnCode) 	LC_YIELD((pt)->lc,ReturnCode)
#define ZT_YIELD1(pt) 				ZT_YIELD2(pt,ZT_YIELDED)
#define	ZT_YIELD0()					ZT_YIELD1(G_CurThread)
#define ZT_YIELD(...)  			 	__PLOOC_EVAL(ZT_YIELD, __VA_ARGS__)(__VA_ARGS__)

#define ZT_YIELD_SLEEP1(pt) 		ZT_YIELD(pt,ZT_SLEEPED)
#define ZT_YIELD_SLEEP0() 			ZT_YIELD_SLEEP1(G_CurThread)
#define ZT_YIELD_SLEEP(...)  		SYS_EVAL(ZT_YIELD_SLEEP, __VA_ARGS__)(__VA_ARGS__)


#define ZT_YIELD_UNTIL2(pt, cond)	do {						\
										ZT_YIELD(pt);				\
										if(!(cond)) {	\
										  return ZT_YIELDED;			\
										}						\
									 } while(0)
#define ZT_YIELD_UNTIL1(cond) 		ZT_YIELD_UNTIL2(G_CurThread,cond)
#define ZT_YIELD_UNTIL(...)  		SYS_EVAL(ZT_YIELD_UNTIL1, __VA_ARGS__)(__VA_ARGS__)

#define ZT_WAIT_WHILE2(pt,cond)  	ZT_WAIT_UNTIL((pt), !(cond))
#define ZT_WAIT_WHILE1(cond)  		ZT_WAIT_WHILE2(G_CurThread,cond)
#define ZT_WAIT_WHILE(...)  		SYS_EVAL(ZT_WAIT_WHILE, __VA_ARGS__)(__VA_ARGS__)

#define ZT_WAIT_THREAD2(pt, thread) ZT_WAIT_WHILE((pt), ZT_SCHEDULE(thread))
#define ZT_WAIT_THREAD1(thread) 	ZT_WAIT_THREAD2(G_CurThread, thread)
#define ZT_WAIT_THREAD(...)  		SYS_EVAL(ZT_WAIT_THREAD, __VA_ARGS__)(__VA_ARGS__)

#define ZT_SPAWN3(pt, child, thread)		\
									  do {						\
										ZT_INIT((child));				\
										ZT_WAIT_THREAD((pt), (thread));		\
									  } while(0)
#define ZT_SPAWN2(child, thread)		ZT_SPAWN3(G_CurThread,child, thread)
#define ZT_SPAWN(...)  					SYS_EVAL(ZT_SPAWN, __VA_ARGS__)(__VA_ARGS__)


#define ZT_RESTART1(pt)					\
									  do {						\
										ZT_INIT(pt);				\
										return ZT_WAITING;			\
									  } while(0)
#define ZT_RESTART0() 				ZT_RESTART1(G_CurThread)
#define ZT_RESTART(...)  			SYS_EVAL(ZT_RESTART, __VA_ARGS__)(__VA_ARGS__)

#define ZT_EXIT1(pt)				\
								  do {						\
									ZT_INIT(pt);				\
									return ZT_EXITED;			\
								  } while(0)
#define ZT_EXIT0() 				ZT_EXIT1(G_CurThread)
#define ZT_EXIT(...)  			SYS_EVAL(ZT_RESTART, __VA_ARGS__)(__VA_ARGS__)

#define ZT_TimeOut1(pt) 			\
							  do {						\
								ZT_INIT(pt);				\
								return ZT_TimeOuted;			\
							  } while(0)
#define ZT_TimeOut0() 		 ZT_TimeOut1(G_CurThread)
#define ZT_TimeOut(...)  	 SYS_EVAL(ZT_TimeOut, __VA_ARGS__)(__VA_ARGS__)

#define ZT_SCHEDULE(f) ((f) <= ZT_WAITING )

#define ZT_IsRunning1(pt) 	LC_IsRunning((pt)->lc)
#define	ZT_IsRunning0()		ZT_IsRunning1(G_CurThread)
#define ZT_IsRunning(...)   SYS_EVAL(ZT_IsRunning, __VA_ARGS__)(__VA_ARGS__)

#endif
/* EOF */