/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� zmDebugManager.h 
�������ڣ�2025-03-28 
�ļ����ܣ���ӦC�ļ���ͷ�ļ�������C�ļ����⹫�����������͡��ӿں�����ȫ�ֱ�����

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

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

#define Debug_EnterCMD				"����ά��ģʽ"

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
//	Debug_Level_None,				//�������Ϣ
//	Debug_Level_Error,				//����쳣��Ϣ���о���ҵ��ʵ�֣�
//	Debug_Level_Main,				//����ص������Ϣ
//	Debug_Level_Log,				//������е�����Ϣ
//}MJ_DebugLevelLX;

/*��ӡһ�м�¼
	RowName-�����ƣ�����Ϊ�ַ���
	RowDataBase-������ָ�룬ָ���1������
	ColumnSpan-ÿһ�����ݵļ������λ��RowDataBase����������
	DataCount-���������ݵ�����
	Format-��ӡ��ʽ����ʽͬ printf �еĴ�ӡ����
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
															
/*��ӡ���ݱ�
	ColumnNameTable-�����Ʊ���һ����������ͷ�е����ƣ��������Ҫ���봫����Ӧ�����Ŀո񣬿ո�ռ�õ��ֽ���Ӧ������ͷ���
	RowNameTable-�����Ʊ�����Ϊ�ַ���ָ�������
	DataTable-������ָ�룬ָ���1������
	ColumnSpan-ÿһ�����ݵļ������λ��DataTable����������
	RowSpan-ÿһ�����ݵļ������λ��DataTable����������,��Ϊ0��������Զ�ʶ���ӡ����
	Format-��ӡ��ʽ����ʽͬ printf �еĴ�ӡ����
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