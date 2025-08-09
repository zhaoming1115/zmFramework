/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： Debug.c 
开发日期：2025-03-28 
文件功能：实现调试功能

描    述：

依赖接口：c_com_t类型的对象，zmSys对象，zmThread对象

提供接口：zmDebugManager对象
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <cmsis_armclang.h>
#include "PAL/PAL.h"
#include "../Sys_DataType.h"
#include "../zmThread/zmThread.h"
#include "../zmDebug/zmDebugManager.h"
#include "../zmSys/zmSys.h"
#include "../zmQueue/zmFrameQueue.h"
#include "DebugFormat.h"

#define __Debug_ValueMaxLength			128
#define __Debug_KeyMaxLength			32

#define DCMD_Exit					"退出"

#define Debug_GetType(Addr,Formatbuf)		int SafeName1=strlen(Formatbuf)-1;\
											((Formatbuf[SafeName1]=='"')?(char*)Addr:(*(int*)Addr))
typedef enum
{
	Debug_Mode_IDLE,
	Debug_Mode_Normal,
	Debug_Mode_High=98,
}e_DebugMode_t;

typedef struct
{
	char AddrKind;		//见e_AddrKind_t
//	unsigned char KeyLength;
	unsigned char ArrayIndex;
	e_BaseDataType_t ValueType;
	char HexFlag;
	char NameFlag;

	union
	{
		unsigned int Address;
		const char* AddrName;
	}Key;
	void* Value;
}s_KeyValuePairInfo_t;

typedef struct
{
	char UserDebugOut;
	char Mode;
	unsigned short KeepSeconds;
	const c_com_t* DefaultDebugPort;
	const c_com_t* DebugPort;
	const s_ElementInfo_t* ElementInfos;
//	const JG_DebugCaoZuoKeyValuePairLX* CaoZuoKeyValueList;
	const s_DebugManagerEvent_t* Event;
	s_StreamInfo_t InStream;
}s_DebugRunParm_t;

static int Debug_Print(const char* data,...);
static void Debug_Enable(void);
static void Debug_Disable(void);
static bool Debug_IsEnable(void);
static int Debug_SendData(const char* data,int DataLength);

static bool g_lock;
//static bool g_EnableFlag __attribute__((section(".bss.NoInitParm")));
static bool g_EnableFlag=false;
static s_DebugRunParm_t g_DebugRunParm;

#define g_DebugPort							(g_DebugRunParm.DebugPort)
#define g_DebugPortManager					g_DebugPort->Manager
#define Debug_OutInfo						((g_DebugRunParm.UserDebugOut)?Debug_Print:printf)
#define Debug_OutBuffer(buf,Length)			(g_DebugRunParm.UserDebugOut)?Debug_SendData(buf,Length):printf(buf)

static int Debug_PickChar(void)
{
	int len;

	char* data= g_DebugPortManager->PickReceivedStream(g_DebugPort,&len);
	return (data==NULL)?EOF:(unsigned char)(*data);	
}


static void Debug_Init(const c_com_t* const DefaultDebugPort,const s_ElementInfo_t* ElementInfos,const s_DebugManagerEvent_t* const DebugManagerEvent )
{
	memset(&g_DebugRunParm,0,sizeof(g_DebugRunParm));
	g_DebugRunParm.DefaultDebugPort=DefaultDebugPort;
	g_DebugRunParm.ElementInfos=ElementInfos;
	g_DebugRunParm.Event=DebugManagerEvent;
	//g_EnableFlag=false;
}

static int Debug_WaitData(s_ProtoThread_t* Thread,int* RCount)
{
	static int rlen=0;
	static unsigned int Tick;

	ZT_BEGIN(Thread);
	Tick=zmSys->GetmTick();	
	
	ZT_WAIT_UNTIL(Thread,(rlen=g_DebugPortManager->ReceiveByteCount(g_DebugPort,true))>0 || zmSys->GetmTickSpan(Tick)>=g_DebugRunParm.KeepSeconds*1000);
	if(rlen<=0) ZT_TimeOut(Thread);
	ZT_SubSleep(Thread,20);
	if(rlen!=g_DebugPortManager->ReceiveByteCount(g_DebugPort,true))
	{
		ZT_EXIT(Thread);
	}
	*RCount=rlen;
	with(&g_DebugRunParm.InStream)
	{
		_->Stream=(unsigned char*)g_DebugPortManager->PickReceivedStream(g_DebugPort,&rlen);
		_->Length=rlen;
		_->UserData=rlen;
	}
	ZT_END(Thread);
}

/*****************************************************************************
* 函 数 名： Debug_AddressKind
* 函数功能： 获取地址类型
* 参    数： Address: 地址
* 返    回： 0-FLASH,1-RAM,其它-无效
* 描    述：

* 开 发 者：赵明 
* 日    期： 2023-04-28

* 修改历史：
** 1. 新创建

 *****************************************************************************/
__WEAK e_AddrKind_t Debug_AddressKind(unsigned int Address)
{
	return (Address==0)?ADDR_Kind_None:((Address&0xf0000000)?ADDR_Kind_RAM:ADDR_Kind_Flash);
}

inline static const s_ElementInfo_t* Debug_GetElementInfoList(void)
{
	return g_DebugRunParm.ElementInfos;
}

__WEAK int Debug_WriteFlash(unsigned int Address,const char* Data,int Length)
{
	return zmStorageManager->Write(Address,Data,Length);
}

__WEAK int Debug_ReadFlash(unsigned int Address,char* Data,int Length)
{
	return zmStorageManager->Read(Address,Data,Length);
}

__WEAK int Debug_WriteConfig(unsigned int Address,const char* Data,int Length)
{
	return Debug_WriteFlash(Address,Data,Length);
}

__WEAK int Debug_ReadConfig(unsigned int Address,char* Data,int Length)
{
	return Debug_ReadFlash(Address,Data,Length);
}

const s_ElementInfo_t* Debug_GetElementInfoWithName(const char* Name)
{
	if(g_DebugRunParm.ElementInfos==NULL) return NULL;
	const s_ElementInfo_t* ElementInfo=g_DebugRunParm.ElementInfos;
	while(Element_InfoIsValid(ElementInfo))
	{
		if(!strncmp(ElementInfo->Name,Name,sizeof(ElementInfo->Name)))
		{
			return ElementInfo;
		}
		ElementInfo++;
	}
	return NULL;
}

#define Debug_ErrorHandler(ReturnBuf,ErrDesc)					memcpy((ReturnBuf),ErrDesc,strlen(ErrDesc)); \
																(ReturnBuf)+=8; \
																if(scanf(",")<0){ \
																	break;}else{ \
																	continue;}

#define Debug_IsReadByName(Value)		(*(Value)=='?')
#define Debug_IsString(Value)			(*(Value)=='"')
#define Debug_SetStringFlag(Value)		(*(Value)='"')
#define Debug_IsAskForRead(Value)		(*(Value)=='%')
#define Debug_IsHex(Value)				(Value[0]=='0' && (Value[1]=='x' || Value[1]=='X'))
#define Debug_IsRead(Value)				(*(Value)=='?' && *(Value)=='%')

static bool Debug_IsDoubleWordData(const char* Value)
{
	const char* flagbase=strchr(Value,'l');
	if(flagbase==NULL) return false;
	flagbase++;
	if((*flagbase)=='l' || (*flagbase)=='f') return true;
	return false;
}

inline static const char* Debug_ValueIsHex(const char* Value)
{
	const char* Flag=strchr(Value,'x');
	Flag=(Flag==NULL)?strchr(Value,'X'):Flag;
	return Flag;
}

static int Debug_GetAddressWithName(s_KeyValuePairInfo_t* KeyValueInfo)
{
	unsigned int Addr=0;
//	char IndexInArray=0;
	int rst=0;

	char* IndexFlag=strchr(KeyValueInfo->Key.AddrName,'[');
	if(IndexFlag)
	{
		int rst=sscanf(IndexFlag+1,"%hhu]",&KeyValueInfo->ArrayIndex);
		if(rst<0) return rst;			
	}
	if(IndexFlag) *IndexFlag=0;
	const s_ElementInfo_t* ElementInfo=Debug_GetElementInfoWithName(KeyValueInfo->Key.AddrName);
	if(ElementInfo)
	{
		if(ElementInfo->Type==DataType_Fun || Element_IsVaildArrayIndex(ElementInfo,KeyValueInfo->ArrayIndex))
		{
			KeyValueInfo->ValueType=ElementInfo->Type;		
			KeyValueInfo->Key.Address=ElementInfo->ElementAddr+Element_GetOffsetAddressWithArrayBase(ElementInfo,KeyValueInfo->ArrayIndex);
			KeyValueInfo->AddrKind=Debug_AddressKind(KeyValueInfo->Key.Address);
		}
		else
		{
			rst=Err_Code_ParmFail;
		}
//		rst=IndexInArray;
	}
	else
	{
		rst=Err_Code_ParmFail;
	}
	if(IndexFlag) *IndexFlag='[';
	return rst;
}


static int Debug_GetKeyInfo(s_KeyValuePairInfo_t* KeyValueInfo,const char* Key)
{
	const char* buf=Key;
	int n=0;
	e_BaseDataType_t Type;
	
	if(Debug_IsHex(buf))
	{
		int rst=sscanf(&buf[2],"%X",&KeyValueInfo->Key.Address);
		if(rst<=0) return rst;
		if(g_DebugRunParm.Mode!=Debug_Mode_High) return -1;	//只有高级权限才可进行地址操作
		KeyValueInfo->AddrKind=Debug_AddressKind(KeyValueInfo->Key.Address);
		KeyValueInfo->HexFlag=1;			
	}
	else
	{
		KeyValueInfo->Key.AddrName=buf;
		return Debug_GetAddressWithName(KeyValueInfo);
	}
//	KeyValueInfo->KeyLength=strlen(Key);
	return 0;
}

static int Debug_AppendFormatAfterKey(char* KeyBuffer,s_KeyValuePairInfo_t* KeyValueInfo)
{

	e_BaseDataType_t DataType=KeyValueInfo->ValueType;
//	char* Key=KeyBuffer+KeyValueInfo->KeyLength;
	char Format[16]={0};

	if(DataType & DataType_StringFlag)
	{
		strcpy(Format,"\"%s\"");
	}
	else if(DataType>=DataType_NumberBase && DataType<=DataType_NumberEnd)
	{
		if(KeyValueInfo->HexFlag)
		{
			int len=Sys_GetDataTypeSize_Number(DataType);
			if(len<=4)
			{
				sprintf(Format,"%%0%dx",len);
			}
			else
			{
				sprintf(Format,"%%0%dllx",len);
			}
		}
		else
		{
			strcpy(Format,g_NumberFormatList[DataType]);
//			*Key++='\n';

		}
	}
	else
	{
		strcpy(Format,"%s");		
	}
	strcat(KeyBuffer,Format);
	return strlen(Format);
}

typedef union
{
	unsigned int InfoValue;
	struct
	{
		e_BaseDataType_t DataType;
		char HexFlag;
		char BaoLiu;
		char Result;	//-1:异常，0-读取，1-写入
	};
}u_ValueInfo_t;
static u_ValueInfo_t Debug_GetNumberType(char* Value)
{
	u_ValueInfo_t ValueInfo={.InfoValue=0};
	
	if(Debug_IsAskForRead(Value))
	{
		if(Debug_IsDoubleWordData(Value))
		{
			ValueInfo.DataType=DataType_Uint64;
		}
		else
		{
			ValueInfo.DataType=DataType_Uint32;
		}
		const char* Hexchar=Debug_ValueIsHex(Value);	//提取16进制格式
		ValueInfo.HexFlag=(Hexchar)?(*Hexchar):0;			
	}
	else
	{
		int n=0;
		int rst;
		unsigned char Data[8];
		int SignedFlag;
		char* buf;
		if(Value[0]=='-')
		{
			buf=&Value[1];
			SignedFlag=1;
		}
		else
		{
			buf=Value;
			SignedFlag=0;
		}
		if(Debug_IsHex(buf))
		{
			rst=sscanf(Value,"%llX%n",(long long*)Data,&n);
			if(rst<1)
			{
				ValueInfo.Result=-1;
				return ValueInfo;
			};
			ValueInfo.DataType=Sys_GetIntegerNumberDataType_Hex(n,SignedFlag);
			ValueInfo.HexFlag='X';
		}
		else
		{
			const char* Point=strchr(Value,'.');		//如果是浮点数
			if(Point)
			{
				rst=sscanf(Value,"%lf%n",(double*)Data,&n);
				if(rst<1)
				{
					ValueInfo.Result=-1;
					return ValueInfo;
				};
				/*如果小数位数小于7位，则使用float，重新处理一下*/
				if(n-(Point-Value)<7)
				{
					*(float*)Data=(float)(*(double*)Data);
					ValueInfo.DataType=DataType_float32;
				}
				else
				{
					ValueInfo.DataType=DataType_float64;
				}
			}
			else
			{
				rst=sscanf(Value,"%lld%n",(long long*)Data,&n);
				if(rst<1)
				{
					ValueInfo.Result=-1;
					return ValueInfo;
				};
				ValueInfo.DataType=Sys_GetIntegerNumberDataType(n,SignedFlag);
			}
		}
		*(long long*)Value=*(long long*)Data;
		ValueInfo.Result=1;
	}
	return ValueInfo;
}

static int Debug_GetValueInfo(s_KeyValuePairInfo_t* KeyValueInfo,char* Value)
{
	char* buf;
	e_BaseDataType_t Type;
	char ValueHexFlag=0;
	
	if(Debug_IsReadByName(Value))
	{
		if(KeyValueInfo->HexFlag)
		{
			strcpy(Value,"%08X");
			KeyValueInfo->ValueType=DataType_Uint32;
		}
		else
		{
			Value[0]=0;
		}
		KeyValueInfo->Value=NULL;	
		return 0;
	}

	if(Debug_IsString(Value))
	{
		if(Debug_IsAskForRead(&Value[1]))
		{
			KeyValueInfo->Value=NULL;
		}
		else
		{
			KeyValueInfo->Value=&Value[1];
		}
		if(KeyValueInfo->ValueType!=DataType_Fun) KeyValueInfo->ValueType=DataType_StringFlag;
		KeyValueInfo->HexFlag=0;
	}
	else if(KeyValueInfo->ValueType==DataType_Fun)
	{
		KeyValueInfo->Value=Value;
	}
	else														//数值型
	{
		u_ValueInfo_t ValueInfo={.InfoValue=0};
		ValueInfo=Debug_GetNumberType(Value);
		if(ValueInfo.Result<0) return Err_Code_ParmFail;
		if(KeyValueInfo->HexFlag)		//如果是使用地址操作，则需要使用从数值分析出来的数据类型
		{
			KeyValueInfo->HexFlag=ValueInfo.HexFlag;
			KeyValueInfo->ValueType=ValueInfo.DataType;
		}
		if(ValueInfo.Result==0)
		{
			KeyValueInfo->Value=NULL;
		}
		else
		{
			KeyValueInfo->Value=Value;
		}
	}
	return 0;
}

inline static int Debug_ReadRAM(unsigned int Address,char* Data,int Length)
{
	memcpy(Data,(char*)Address,Length);
	return Length;
}

static int Debug_ReadKeyValue(s_KeyValuePairInfo_t* KeyValueInfo,const char** ErrDesc)
{
	int (*Debug_ReadFunList[])(unsigned int Address,char* Data,int Length)={[ADDR_Kind_RAM]=Debug_ReadRAM,
																			[ADDR_Kind_Flash]=Debug_ReadFlash,
																			[ADDR_Kind_Config]=Debug_ReadConfig};
	int size;
	int rst=-1;
	if(KeyValueInfo->ValueType==DataType_Fun)
	{
		f_Element_Fun_t fun=(f_Element_Fun_t)KeyValueInfo->Key.Address;
		char* Value=(char*)KeyValueInfo->Value;
		Value[4]=0;
		*(unsigned int*)KeyValueInfo->Value=(unsigned int)&Value[4];
		rst=fun((int)(char)KeyValueInfo->ArrayIndex,&Value[4],ErrDesc);
		if(rst<0) return rst;
		rst=4;
	}
	else if(Debug_IsValidAddrKind(KeyValueInfo->AddrKind))
	{
		if(KeyValueInfo->ValueType<0)	//字符串，将其地址作为数据使用
		{
			*(unsigned int*)KeyValueInfo->Value=KeyValueInfo->Key.Address;
			rst=4;
		}
		else if(KeyValueInfo->ValueType==DataType_float32)
		{
			*(double*)KeyValueInfo->Value=*(float*)KeyValueInfo->Key.Address;
			return 8;
		}
		else if(KeyValueInfo->ValueType<DataType_DoubleWordNumberBase)	//数值类型按照32位处理
		{
			Debug_ReadFunList[KeyValueInfo->AddrKind](KeyValueInfo->Key.Address,KeyValueInfo->Value,4);
			rst=4;
		}
		else if(KeyValueInfo->ValueType<=DataType_NumberEnd)	//数值类型按照64位处理
		{
			Debug_ReadFunList[KeyValueInfo->AddrKind](KeyValueInfo->Key.Address,KeyValueInfo->Value,8);
			rst=8;
		}
		else	//其它的按照字符串或者字节流处理，将其地址作为数据使用
		{
			*(unsigned int*)KeyValueInfo->Value=KeyValueInfo->Key.Address;
			rst=4;
		}
	}	
	return rst;
}


inline static int Debug_WriteRAM(unsigned int Address,const char* Data,int Length)
{
	memcpy((char*)Address,Data,Length);
	return Length;
}

static int Debug_WriteKeyValue(s_KeyValuePairInfo_t* KeyValueInfo,const char** ErrDesc)
{
	int (*Debug_WriteFunList[])(unsigned int Address,const char* Data,int Length)={[ADDR_Kind_RAM]=Debug_WriteRAM,
																			[ADDR_Kind_Flash]=Debug_WriteFlash,
																			[ADDR_Kind_Config]=Debug_WriteConfig};
	int len;
	int rst=-1;
	if(KeyValueInfo->ValueType==DataType_Fun)
	{
		f_Element_Fun_t fun=(f_Element_Fun_t)KeyValueInfo->Key.Address;
		char* Value=(char*)KeyValueInfo->Value;
		return fun(KeyValueInfo->ArrayIndex,Value,ErrDesc);		
	}
	if(Debug_IsValidAddrKind(KeyValueInfo->AddrKind))
	{
		if(KeyValueInfo->ValueType & DataType_StringFlag)
		{
			char* String=(char*)KeyValueInfo->Value;
			len=strlen(String);
			if(Debug_IsString(&String[len-1])) len--;	//如果最后一个字符是双引号，则将字符串长度减1
			len++;	//后面写字符串是以16进制的方式写的，因此长度必须包含后面的0
//			int maxlen=Sys_GetDataTypeSize_String(KeyValueInfo->ValueType);
//			if(len>maxlen) 
//			{
//				len=maxlen;
//			}
			String[len-1]=0;		//字符串的最后一个字符必须为0
		}
		else if(KeyValueInfo->ValueType<=DataType_NumberEnd)	//数值型的数据
		{
			len=Sys_GetDataTypeSize_Number(KeyValueInfo->ValueType);
		}
		else	//其它的按照字节流处理
		{
			len=KeyValueInfo->ValueType;
		}
		rst=Debug_WriteFunList[KeyValueInfo->AddrKind](KeyValueInfo->Key.Address,KeyValueInfo->Value,len);
	}	
	return rst;
}

static int Debug_SwapData(char* Key,char* Value,const char** ErrDesc)
{
	s_KeyValuePairInfo_t __KeyValueInfo={0};
	s_KeyValuePairInfo_t* KeyValueInfo=&__KeyValueInfo;
	KeyValueInfo->ValueType=DataType_StringFlag;
	int rst=Debug_GetKeyInfo(KeyValueInfo,Key);
	if(rst<0) return -1;
	rst=Debug_GetValueInfo(KeyValueInfo,Value);
	if(rst<0) return -1;
	
	int KeyLength=strlen(Key);
	e_BaseDataType_t SrcDataType=DataType_Fun;
	/*如果值指针不为空，说明是写入请求，否则就是读取请求*/
	if(KeyValueInfo->Value)
	{
		rst=Debug_WriteKeyValue(KeyValueInfo,ErrDesc);
		if(rst<0) return rst;		
	}
	else
	{
		char* Format=&Key[KeyLength+1];
		if(KeyValueInfo->HexFlag) 
		{
			*Format++='0';
			*Format++='X';
		}
		strcpy(Format,Value);
	}
	KeyValueInfo->Value=Value;
	rst=Debug_ReadKeyValue(KeyValueInfo,ErrDesc);
	if(rst<0) return rst;
	
	Key[KeyLength++]='=';
	if(Key[KeyLength]==0)
	{
		Debug_AppendFormatAfterKey(Key,KeyValueInfo);
	}
	strcat(Key,"\r\n");
	return rst;
}

static int Debug_ExeFun(const char* Key,const char** ErrDesc)
{
	s_KeyValuePairInfo_t __KeyValueInfo={0};
	s_KeyValuePairInfo_t* KeyValueInfo=&__KeyValueInfo;
	int rst=Debug_GetKeyInfo(KeyValueInfo,Key);
	if(rst<0)
	{
		*ErrDesc="解析函数名称失败";
		return Err_Code_ParmFail;
	}
	if(!KeyValueInfo->HexFlag) 
	{
		int rst=Debug_GetAddressWithName(KeyValueInfo);
		if(rst<0)
		{
			*ErrDesc="不支持的函数";
			return Err_Code_ParmFail;
		}
	}
	const char* ParmString=strchr(Key,'(');
	if(ParmString) ParmString++;
	f_Element_Fun_t Fun=(f_Element_Fun_t)KeyValueInfo->Key.Address;
	return Fun(KeyValueInfo->ArrayIndex,ParmString,ErrDesc);
	
}

static int Debug_OutSupportElementInfo(const s_ElementInfo_t* ElementInfo,int StartIndex)
{
	#define NameFormat					"?"
	int i=StartIndex;
	char buf[256];

	while(Element_InfoIsValid(ElementInfo))
	{
		int len=sprintf(buf,"%d-",i);
		len+=zmElementInfoManager->ToString(ElementInfo,&buf[len]);
		buf[len++]='\n';
		buf[len]='\0';
		Debug_OutInfo(buf);
		ElementInfo++;
		i++;
	}
	return i;
}


static void Debug_OutFormat(void)
{
//Debug_GetElementInfoList
	Debug_OutInfo("\n===============调试信息输入输出格式======================\n");
	Debug_OutBuffer(g_DebugFormat,sizeof(g_DebugFormat)-1);

	Debug_OutInfo("\n键值对列表：\n");
	int MaxCount=0;
	int StartIndex=0;
	const s_ElementInfo_t* ElementInfo=Debug_GetElementInfoList();
	if(ElementInfo) StartIndex=Debug_OutSupportElementInfo(ElementInfo,StartIndex);

	if(g_DebugRunParm.Mode==Debug_Mode_High)
	{
//		ElementInfo=Debug_GetElementInfoList();
//		if(ElementInfo) Debug_OutSupportElementInfo(ElementInfo,StartIndex-1);
		Debug_OutInfo("\n以上元素名均可以使用32位地址替代，同时亦可指定数值类型，如:\n");
		Debug_OutInfo("0x20000000=%%d\n");		
	}

	Debug_OutInfo("\n========================================================\n");
}

static char* Debug_TrimString(char* String)
{
	while(String)
	{
		switch(*String)
		{
			case ' ':
			case '\n':
			case '\r':
//			case '\0':
				String++;
				break;

			default:
				return String;
		}
	}
	return String;
}

static int Debug_DebugProcess(void)
{
#define EnterDebugCMD			"调试功能"

	static s_ProtoThread_t _subThread;
	static unsigned int LastRTick=0;
	int RCount=0;

	ZT_BEGIN();
	g_DebugRunParm.Mode=0;
	g_DebugRunParm.KeepSeconds=60;
	Debug_OutInfo("等待调试指令\n");
	do
	{
		if((g_DebugPortManager->ReceiveByteCount(g_DebugPort,true)<=0)) break;
	}while(1);
/*等待调试功能开启*/
	while(g_DebugRunParm.Mode==0)
	{
		int tmp;
		ZT_SPAWN(&_subThread,(tmp=Debug_WaitData(&_subThread,&RCount)));
		if(tmp==ZT_ENDED)
		{
			if(RCount>sizeof(EnterDebugCMD)+4 && RCount<256)
			{
				int Mode;
				
				char Buf[256];
				memset(Buf,0,sizeof(Buf));
				tmp=scanf("%[^=]%d%hd",Buf,&Mode,&g_DebugRunParm.KeepSeconds);
				if(tmp>0 && !strncmp(Buf,Debug_EnterCMD,sizeof(Debug_EnterCMD)-1))
				{
					Debug_OutInfo("已经处于维护模式中了\n");								
				}
				else if(tmp==3 && !strcmp(Buf,EnterDebugCMD))
	//			tmp=scanf("调试功能=%d%hd",&Mode,&g_DebugRunParm.KeepSeconds);
	//			if(tmp==2)
				{
					switch(Mode)
					{
						case Debug_Mode_Normal:
						case Debug_Mode_High:
							g_DebugRunParm.Mode=Mode;
							break;
						
						default:
							g_DebugRunParm.Mode=0;
							break;
					}
				}
			}
			g_DebugPortManager->Read(g_DebugPort,NULL,COM_DataLength_OneFrame);		//清空缓存区
		}
		ZT_Sleep(10);
	}
	
	Debug_OutInfo("调试功能已经开启，请按照如下格式进行操作，若连续%d秒无操作，将会自动关闭\n",g_DebugRunParm.KeepSeconds);
	Debug_OutFormat();

	while(g_DebugRunParm.Mode)
	{
		int tmp;
		LastRTick=zmSys->GetmTick();
		ZT_SPAWN(&_subThread,(tmp=Debug_WaitData(&_subThread,&RCount)));
		if(tmp==ZT_TimeOuted) 
		{
			Debug_OutInfo("超时\n");
			break;
		}
		tmp=EOF;
		
//		SPrintfFun_us(({
			do
			{
				char Buf[1024];
				char* Buf_Key=Buf;
				char* Buf_Value=&Buf[64];
				memset(Buf,0,sizeof(Buf));
				tmp=scanf("%[^=]%[^\r\n]",Buf_Key,Buf_Value);
				Buf[sizeof(Buf)-1]=0;
				Buf_Key=Debug_TrimString(Buf_Key);
				if(tmp==2)
				{
					const char* ErrDesc=NULL;
					Buf_Key[63]=0;
					int rst= Debug_SwapData(Buf_Key,Buf_Value,&ErrDesc);
					if(rst>4)
					{
						Debug_OutInfo(Buf_Key,*(long long*)Buf_Value);
					}
					else if(rst>=0)
					{
						Debug_OutInfo(Buf_Key,*(int*)Buf_Value);
					}
					else
					{
						if(!strncmp(Buf_Key,"调试功能",8))
						{
							Debug_OutInfo("调试功能已经开启\n");								
						}
						else if(!strncmp(Buf_Key,Debug_EnterCMD,sizeof(Debug_EnterCMD)-1))
						{
							Debug_OutInfo("已经处于维护模式中了\n");								
						}
						else
						{
							Debug_OutInfo("%s=异常(%d),异常描述：%s\n", Buf_Key,rst,(ErrDesc)?ErrDesc:"无");
						}
					}
				}
				else if(tmp==1)
				{
					if(strlen(Buf_Key)<=0) continue;
					if(!strncmp(Buf_Key,"退出",4))
					{
						g_DebugRunParm.Mode=0;
					}
					const char* Errdesc=NULL;
					int rst=Debug_ExeFun(Buf_Key,&Errdesc);
					if(rst>=0)
					{
						Debug_OutInfo("%s=成功\n", Buf_Key);
					}
					else
					{
						Debug_OutInfo("%s=失败，异常码：%d,异常描述：%s\n", Buf_Key,rst,(Errdesc)?Errdesc:"无");
					}
					
				}
				else if(tmp>0)
				{
					Debug_OutInfo("%s=格式不正确\n", Buf_Key);
				}
			}while(tmp>0);
//}));
			g_DebugPortManager->Read(g_DebugPort,NULL,COM_DataLength_OneFrame);		//清空缓存区
	}

	Debug_OutInfo("成功退出调试模式\n");
	
	ZT_END();	
}

static int Debug_SwapDatas(const char* InStream,char* OutStream)
{
	char* To=OutStream;
	const char* From= InStream;
	
	while(*From)
	{
		char Buf[1024];
		char* Buf_Key=Buf;
		char* Buf_Value=&Buf[64];
		memset(Buf,0,sizeof(Buf));
		int len=0;
		int tmp=sscanf(InStream,"%[^=]=%s%n",Buf_Key,Buf_Value,&len);
		if(tmp<0 || len<=0) break;
		Buf[sizeof(Buf)-1]=0;
		Buf[63]=0;
		Buf_Key=Debug_TrimString(Buf_Key);
		if(tmp==2)
		{
			const char* ErrDesc=NULL;
			int rst= Debug_SwapData(Buf_Key,Buf_Value,&ErrDesc);
			if(To)
			{
				if(rst>4)
				{
					To+=sprintf(To,Buf_Key,*(long long*)Buf_Value);
				}
				else if(rst>=0)
				{
					To+=sprintf(To,Buf_Key,*(int*)Buf_Value);
				}
				else
				{
					To+=sprintf(To,"%s=异常(%d),异常描述：%s\n", Buf_Key,rst,(ErrDesc)?ErrDesc:"无");
				}
			}
		}
		else if(tmp==1)
		{
			const char* Errdesc=NULL;
			int rst=Debug_ExeFun(Buf_Key,&Errdesc);
			if(To)
			{
				if(rst>=0)
				{
					To+=sprintf(To,"%s=成功\n", Buf_Key);
				}
				else
				{
					To+=sprintf(To,"%s=失败，异常码：%d,异常描述：%s\n", Buf_Key,rst,(Errdesc)?Errdesc:"无");
				}
			}
		}
		else if(tmp>0)
		{
			if(To)
			{
				To+=sprintf(To,"%s=格式不正确\n", Buf_Key);
			}
		}
		InStream+=len;
	};
	if(To)
	{
		*To=0;
	}
	return To-OutStream;
}

static void Debug_Disable(void)
{
	g_DebugRunParm.Mode=0;
	g_DebugPortManager->Close(g_DebugPort);
	zmThreadManager->RemoveThreadWithProcess(Debug_DebugProcess);
	g_EnableFlag=false;
}

static void Debug_Start(const c_com_t* const DebugPort)
{
	if(DebugPort)
	{
		g_DebugRunParm.UserDebugOut=1;
		g_DebugRunParm.DebugPort=DebugPort;
	}
	else if(g_DebugRunParm.DefaultDebugPort)
	{
		g_DebugRunParm.DebugPort=g_DebugRunParm.DefaultDebugPort;
		g_DebugRunParm.UserDebugOut=0;
	}
	else
	{
		return;
	}
	if(g_EnableFlag)
	{
		Debug_Disable();
//		g_DebugRunParm.Mode=0;
	}
	g_DebugPortManager->Open(g_DebugPort,NULL);
	zmThreadManager->CreateAddThread(Debug_DebugProcess,Thread_YXJ_CallByLunXun,false);
	g_EnableFlag=true;
}

static bool Debug_IsEnable(void)
{
	return g_EnableFlag;
}

static int Debug_SendData(const char* data,int DataLength)
{
	int tmp;
//	static bool g_lock;

	if (g_EnableFlag==false) return -1;
	if(g_lock)return -2;
	g_lock=true;
	while(DataLength>0)
	{
		int len;
		if(g_DebugRunParm.Event && g_DebugRunParm.Event->SendData)
		{
			len= g_DebugRunParm.Event->SendData(data,DataLength);
		}
		else
		{
			len=g_DebugPortManager->Write(g_DebugPort,data,DataLength);
		}
		if(len>0)
		{
			DataLength-=len;
		}
		else
		{
			SPrintf(2,"等待缓存区\n");
			zmSys->mDelay(1);
		}
	}
//	free(p);
	g_lock=false;
	return DataLength;
}

static int Debug_Print(const char* data,...)
{
	va_list vArgList; 
	int tmp;
	
//	static bool g_lock;

	if(g_DebugRunParm.UserDebugOut)
	{	
		char buffer[256];
		va_start (vArgList, data); 
		tmp=vsnprintf(buffer, sizeof(buffer), data, vArgList);
		va_end(vArgList);
		if(tmp>sizeof(buffer)) tmp=sizeof(buffer);
		return Debug_SendData(buffer,tmp);
	}
	else
	{
		va_start (vArgList, data); 
		int rst=vprintf(data,vArgList);
		va_end(vArgList);
		return rst;
	}
}

static int Debug_CHeckEnter(const c_com_t* const Port,const char* Data,const char* ParmFormat,...)
{
	int len=sizeof(Debug_EnterCMD)-1;
	int tmp=(strncmp(Data,Debug_EnterCMD,len))?-1:0;
	if(tmp) return tmp;
	if(ParmFormat)
	{
		va_list vArgList; 
		va_start (vArgList, ParmFormat); 
		int parm[2];
		tmp=vsscanf(Data+len,ParmFormat,vArgList);
		va_end(vArgList);
	}
	return tmp;
}

const i_DebugManager_t* const zmDebugManager=&(const i_DebugManager_t){.Init=Debug_Init,
								.Enable= Debug_Start,
								.Disable=Debug_Disable,
								.IsEnable=Debug_IsEnable,
								.Write=Debug_SendData,
								.Print=Debug_Print,
								.CHeckEnter=Debug_CHeckEnter,
								.SwapDatas=Debug_SwapDatas};

int fgetc(FILE *f)
{
	unsigned char data;
	with(&g_DebugRunParm.InStream)
	{		
		if(_->Length<=0)
		{
			int len;
			g_DebugPortManager->Read(g_DebugPort,NULL,_->UserData);
			_->Stream=(unsigned char*)g_DebugPortManager->PickReceivedStream(g_DebugPort,&len);
			_->Length=len;
			_->UserData=len;
		}
		if(_->Stream)
		{
			_->Length--;
			return *_->Stream++;
		}
	}
	return EOF;
}

