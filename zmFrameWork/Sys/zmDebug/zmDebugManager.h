/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： zmDebugManager.h 
开发日期：2025-03-28 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __ZM_DEBUG_MANAGER_H_
#define __ZM_DEBUG_MANAGER_H_

//#include "PAL/Inc/IPort.h"
#include "/PAL/PAL.h"
#include "stdbool.h"
#include <stdio.h>
#include <string.h>
#include "zmDebug.h"
#include "../zmElementInfo/zmElementInfo.h"

#define Debug_EnterCMD				"进入维护模式"

typedef enum		
{
	ADDR_Kind_None=-1,
//	ADDR_Kind_Name,
	ADDR_Kind_Base,
	ADDR_Kind_RAM=ADDR_Kind_Base,
	ADDR_Kind_Flash,
	ADDR_Kind_Config,
	ADDR_Kind_Count
#define Debug_IsValidAddrKind(ADDR_Kind)		(ADDR_Kind>=ADDR_Kind_Base && ADDR_Kind<ADDR_Kind_Count)
}e_AddrKind_t;

typedef struct
{
	int (*SendData)(const char* data,int datalength);
}s_DebugManagerEvent_t;

typedef struct
{
	void (*Init)(const c_com_t* const Port,const s_ElementInfo_t* ElementInfos, const s_DebugManagerEvent_t* const DebugManagerEvent);
	void (*Enable)(const c_com_t* const Port);
	void (*Disable)(void);
	bool (*IsEnable)(void);
	int (*Write)(const char* data,int datalength);
	int (*Print)(const char* data,...);
	int (*CHeckEnter)(const c_com_t* const Port,const char* Data,const char* ParmFormat,...);
	int (*SwapDatas)(const char* InStream,char* OutStream);
}i_DebugManager_t;


#pragma pack()

//extern const JG_DebugManagerEventLX g_DebugManagerEvent;

extern const i_DebugManager_t* const zmDebugManager;


inline static int Debug_CHeckAndEnter(const c_com_t* const Port,char* Data,int length)
{
	Data[length]=0;
	int tmp=zmDebugManager->CHeckEnter(Port,Data,NULL);
	if(tmp>=0) zmDebugManager->Enable(Port);
	return tmp;
}

int SPrintfRow(const char* const RowName,const char* const *ColumnNameTable,const void** const RowDataBase);
int SPrintfTable(const char* const *RowNameTable,const char* const *ColumnNameTable,const void* DataBase);

//typedef enum
//{
//	Debug_Level_None,				//不输出信息
//	Debug_Level_Error,				//输出异常信息（有具有业务实现）
//	Debug_Level_Main,				//输出重点调试信息
//	Debug_Level_Log,				//输出所有调试信息
//}MJ_DebugLevelLX;

/*打印一行记录
	RowName-行名称，必须为字符串
	RowDataBase-行数据指针，指向第1个数据
	ColumnSpan-每一列数据的间隔，单位是RowDataBase的数据类型
	DataCount-本行中数据的数量
	Format-打印格式，格式同 printf 中的打印个数
*/
#define SPrintf_Row(RowName,ColumnNameTable,RowDataBase,ColumnSpan,DataCount,Format) (ColumnSpan<=0)?SPrintfRow(RowName,ColumnNameTable,(const void** const)RowDataBase):\
																					({SPrintf_2("%s ",RowName); \
																						for(int ID=0;ID<(DataCount)*(ColumnSpan);ID+=(ColumnSpan)){\
																							SPrintf_2(Format,(RowDataBase)[ID]);} \
																						SPrintf_2("\n");})

																		
#define SPrintf_Row_User(RowName,ColumnNameTable,RowDataBase,ColumnSpan,DataCount,UserFun) (ColumnSpan<=0)?SPrintfRow(RowName,ColumnNameTable,(const void** const)RowDataBase):\
																					({SPrintf_2("%s ",RowName); \
																						for(int ID=0;ID<(DataCount)*(ColumnSpan);ID+=(ColumnSpan)){\
																							UserFun(&(RowDataBase)[ID]);} \
																						SPrintf_2("\n");})
															
/*打印数据表
	ColumnNameTable-列名称表，第一个名称是行头列的名称，如果不需要，请传入相应数量的空格，空格串占用的字节数应该与行头相符
	RowNameTable-行名称表，必须为字符串指针的数组
	DataTable-表数据指针，指向第1个数据
	ColumnSpan-每一列数据的间隔，单位是DataTable的数据类型
	RowSpan-每一行数据的间隔，单位是DataTable的数据类型,若为0，则调用自动识别打印函数
	Format-打印格式，格式同 printf 中的打印个数
*/
#define SPrintf_Table(ColumnNameTable,RowNameTable,DataTable,ColumnSpan,RowSpan,Format) (RowSpan<=0)?SPrintfTable(RowNameTable,ColumnNameTable,DataTable): \
																						({SPrintf_2("\n"); \
																						SPrintf_Row(ColumnNameTable[0],&ColumnNameTable[1],&ColumnNameTable[1],1,Array_GetCount(ColumnNameTable)-1,"%s "); \
																						for(int RID=0;RID<Array_GetCount(RowNameTable);RID++){ \
																							SPrintf_Row(RowNameTable[RID],&ColumnNameTable[1],&(DataTable)[RID*(RowSpan)],ColumnSpan,Array_GetCount(ColumnNameTable)-1,Format);};})

#define SPrintf_Table_User(ColumnNameTable,RowNameTable,DataTable,ColumnSpan,RowSpan,UserFun) (RowSpan<=0)?SPrintfTable(RowNameTable,ColumnNameTable,DataTable): \
																						({SPrintf_2("\n"); \
																						SPrintf_Row(ColumnNameTable[0],&ColumnNameTable[1],&ColumnNameTable[1],1,Array_GetCount(ColumnNameTable)-1,"%s "); \
																						for(int RID=0;RID<Array_GetCount(RowNameTable);RID++){ \
																							SPrintf_Row_User(RowNameTable[RID],&ColumnNameTable[1],&(DataTable)[RID*(RowSpan)],ColumnSpan,Array_GetCount(ColumnNameTable)-1,UserFun);};})


#endif