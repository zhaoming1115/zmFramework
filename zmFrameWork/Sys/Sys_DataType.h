/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� Sys_DataType.h 
�������ڣ�2025-04-09 
�ļ����ܣ����õ��������͡�

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/

 #ifndef __SYS__DATA_TYPE_H_
#define __SYS__DATA_TYPE_H_

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#define __Out
#define __In									const
#define __InOut									
#define __Aligned(__N)							__attribute__((aligned(__N)))

typedef unsigned long long						uint64;
typedef long long								int64;

#define __HongToString(Hong)		#Hong
#define HongToString(Hong)			__HongToString(Hong)

/*���������ַ���*/
#define ConName(Str1,Str2)						Str1##Str2
//#define NewName(Header)							ConName(Header,__LINE__)
#define SafeName(Name)							ConName(Name,__LINE__)
#define Array_GetCount(Array)							(sizeof(Array)/sizeof(Array[0]))
#define SizeofStructMember(Struct,Member)				((int)sizeof(((Struct*)0)->Member))
/*�����ṹ���Ա֮��ļ��
���=��2����Ա��ƫ����-��һ����Ա��ƫ����+��һ����Ա�Ĵ�С
*/
#define SpanofStructMember(Struct,Member1,Member2)		(int)(offsetof(Struct,Member2)-offsetof(Struct,Member1)-SizeofStructMember(Struct,Member1))


/*================================================ ϵͳ�������ͼ��꺯�� =========================================================*/
#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

#define DecToBCD(Dec)			(((Dec) % 10)+(((Dec)/10)<<4))
#define BCDToDec(BCD)			(((BCD) & 0XF)+((((unsigned char)BCD)>>4) * 10)

typedef enum
{
	Err_Code_Storage_Base=-16*3,
	Err_Code_Storage_Fail=Err_Code_Storage_Base,
	Err_Code_Storage_WriteFail,
	Err_Code_Storage_EraseFail,
	Err_Code_SysBase=-16*2,
	Err_Code_ParmFail,
	Err_Code_Busy,
	Err_Code_AskMemoryFail,	//�����ڴ�ʧ��
	Err_Code_ThreadReturnBase=-16,
}e_ErrorCode_t;

#define Sys_Assert_Param(__EXPR)			if(__EXPR) return Err_Code_ParmFail

typedef enum
{
	Sys_StartFrom_PowerOn,		//�ϵ�����
	Sys_StartFrom_HWRst,		//Ӳ����λ
	Sys_StartFrom_SWRst,		//�����λ
	Sys_StartFrom_WDG,		//���Ź���λ
	Sys_StartFrom_WakeUpFromStandby,		//��������
	Sys_StartFrom_Other
}e_Sys_StartFrom_t;

typedef enum
{
	DataType_Fun=0,
	DataType_Uint8,
	DataType_int8,
	DataType_Uint16,

	DataType_int16,
	DataType_Uint32,
	DataType_int32,
	DataType_float32,

	DataType_Uint64,
	DataType_int64,
	DataType_float64,
	DataType_ByteArrayBase,
	DataType_ByteArrayEnd=0x7F,
	DataType_NumberBase=DataType_Uint8,
	DataType_NumberEnd=DataType_float64,
	DataType_DoubleWordNumberBase=DataType_Uint64,
	DataType_StringFlag=-0x80,			//���������ַ����������ֵ�����ַ�������,����-0x80����̬�ַ������������Ϊ\0
}e_BaseDataType_t;

static const char* const g_NumberFormatList[]={"%p","%hhu","%c","%hu","%hd","%u","%d","%f","%llu","%lld","%lf"};

inline static int Sys_GetDataTypeSize_String(e_BaseDataType_t DataType)
{
	return -DataType;
}

#define Sys_MakeStringType(Size)					(-Size)



inline static int Sys_GetDataTypeSize_Number(e_BaseDataType_t DataType)
{
	static const char __DataTypeSizeList[]={4,1,1,2,2,4,4,4,8,8,8};
	return __DataTypeSizeList[DataType];
}

inline static int Sys_GetDataTypeSize(e_BaseDataType_t DataType)
{
	int Size=(DataType <0)?Sys_GetDataTypeSize_String(DataType):
			(DataType==DataType_Fun)?4:
			(DataType<DataType_NumberEnd)?Sys_GetDataTypeSize_Number(DataType):DataType;
	return Size;
}

inline static e_BaseDataType_t Sys_GetIntegerNumberDataType(int NumberLenInString,char SignedFlag)
{
	NumberLenInString-=SignedFlag;
	e_BaseDataType_t DataType=(NumberLenInString<3)?DataType_Uint8:
								(NumberLenInString<6)?DataType_Uint16:
								(NumberLenInString<10)?DataType_Uint32:DataType_Uint64;
	DataType+=SignedFlag;
	return DataType;
}

inline static e_BaseDataType_t Sys_GetIntegerNumberDataType_Hex(int NumberLenInString,char SignedFlag)
{
	NumberLenInString-=SignedFlag;
	e_BaseDataType_t DataType=(NumberLenInString<3)?DataType_Uint8:
								(NumberLenInString<5)?DataType_Uint16:
								(NumberLenInString<9)?DataType_Uint32:DataType_Uint64;
	DataType+=SignedFlag;
	return DataType;
}

typedef struct
{
	char CMD;		//�������쳣�������෴����0-�������У�1-�ӻ�������������2-�ָ�����һ�汾�ĳ���3-�ָ�����������4-��Ĭ�ϵ�ͨ�ſڽ��ճ�������
	char State;		//������������Boot�У�����-������App��
	char ErrorCode;	//�쳣����
	char PerErrorCode;
}s_BootRunParm_t;


//����������
typedef struct
{
	unsigned short UserData;
	unsigned short Length;
	unsigned char* Stream;
}s_StreamInfo_t;

//������������
typedef struct
{
	unsigned short ElementSize;
	unsigned short ElementCount;
	void* Element;
}JG_ArrayInfoLX;

typedef struct  
{
	unsigned int Address;
	int Length;
}JG_DataRegionLX;
#define DataRegion_Append(DataAddr,DataSize)	{(DataAddr),(DataSize)}


#define Array_ElementSize_Auto					0
#define SizeStringOf(Size)	char 
#define JG_KeyValuePairOf(TKey,TValue)		struct{	\
												TKey Key;	\
												TValue Value;}

#define JG_StreamOf(__T)		struct{	\
								unsigned short UserData;	\
								unsigned short Length;	\
							__T Data[];}

#define s_ArrayOf(__T)		struct{	\
								unsigned short ItemSize;	\
								unsigned short ItemCount;	\
								__T Items[];}

#define Append_ArrayOf(__T,...)								{.ItemSize=sizeof(__T),.ItemCount=sizeof((__T[]){__VA_ARGS__})/sizeof(__T),.Items={__VA_ARGS__}}

#define s_SizeArrayOf(T,Size)		struct{	\
										unsigned short ItemSize;	\
										unsigned short ItemCount;	\
										T Items[(Size)];}

typedef JG_StreamOf(unsigned char)	JG_StreamLX;
typedef JG_StreamOf(int)			JG_intStreamLX;

typedef JG_StreamOf(char)			string;
typedef s_ArrayOf(char)			array;

typedef JG_KeyValuePairOf(void*,void*) JG_KeyValuePairLX;

inline static string* String_New(int Length)
{
	string* str=malloc((unsigned int)Length+4);
	memset(str,0,Length+4);
	str->Length=(unsigned short)Length;
	return str;
}

inline static void String_Dispose(string* str)
{
	free(str);
	str=NULL;
}


#define JG_SizeStreamOf(T,Size)		struct{	\
										unsigned short UserData;	\
										unsigned short Length;	\
										T Data[(Size)];}

#define JG_SizeByteStreamOf(Size)	JG_SizeStreamOf(unsigned char,((Size)+3)&~0x3)		//����4�ֽڶ���
#define JG_SizeWordStreamOf(Size)	JG_SizeStreamOf(int,Size)
		
//#define SizeString_New(Size)			{0,Size,}		

#define Sys_ProgramVersionCodeLength				16
typedef struct
{
	char Model[32];	//��Ʒ�ͺ�
	char ID[32];		//��ƷID��IDΪ������̬��¼
	char CHangSHang[8];	//���̴���
	char VSoft[Sys_ProgramVersionCodeLength];	//����汾
	char SoftDate[8];	//����������ڣ�ʵ��ʹ��6���ֽڣ�����ʹ��8��Ϊ��4�ֽڶ��룬�ұ���4�ֽڶ���
	char VHard[Sys_ProgramVersionCodeLength];	//Ӳ���汾
	char HardDate[8]; //Ӳ���������ڣ�ʵ��ʹ��6���ֽڣ�����ʹ��8��Ϊ��4�ֽڶ��룬�ұ���4�ֽڶ���
	char ExtendInfo[8];	//������չ��Ϣ
}s_ProductInfo_t;		//��Ʒ��Ϣ����

#define G_ProductInfoElementNameS(PreName,AfterName)	PreName"��Ʒ�ͺ�"AfterName,PreName"��Ʒ���"AfterName,PreName"���̴���"AfterName,PreName"����汾"AfterName, \
														PreName"�����������"AfterName,PreName"Ӳ���汾"AfterName,PreName"Ӳ����������"AfterName,PreName"������չ��Ϣ"AfterName
extern const s_ProductInfo_t G_ProductInfo;		//��Ʒ��Ϣ���̻��������С�ȫ�ֵ���


/*================================================ ������Ϣ ======================================================================*/
#if DisAutoBuildTime
#define Bulid_YEAR			2023
#define Bulid_MONTH			2
#define Bulid_DAY			1
#define Bulid_HOUR			15
#define Bulid_MINUTE		14
#define Bulid_SECOND		45
#else

#define Bulid_YEAR (((__DATE__ [7] - '0')*1000) +((__DATE__ [8] - '0')*100) +((__DATE__ [9] - '0')*10) + (__DATE__ [10] - '0'))

#define Bulid_MONTH ( __DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 0X01 : 0X06) \
			: __DATE__ [2] == 'b' ? 0X02 \
			: __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 0X03 : 0X04) \
			: __DATE__ [2] == 'y' ? 0X05 \
			: __DATE__ [2] == 'n' ? 0X06 \
			: __DATE__ [2] == 'l' ? 0X07 \
			: __DATE__ [2] == 'g' ? 0X08 \
			: __DATE__ [2] == 'p' ? 0X09 \
			: __DATE__ [2] == 't' ? 10 \
			: __DATE__ [2] == 'v' ? 11 : 12)

#define Bulid_DAY ((__DATE__ [4] == ' ' ? 0 : ((__DATE__ [4] - '0')*10)) + (__DATE__ [5] - '0'))

#define Bulid_HOUR ((__TIME__ [0] == ' ' ? 0 : ((__TIME__ [0] - '0')*10)) + (__TIME__ [1] - '0'))
#define Bulid_MINUTE		((__TIME__ [3] == ' ' ? 0 : ((__TIME__ [3] - '0')*10)) + (__TIME__ [4] - '0'))
#define Bulid_SECOND		((__TIME__ [6] == ' ' ? 0 : ((__TIME__ [6] - '0')*10)) + (__TIME__ [7] - '0'))


//static const unsigned int G_BulidTime=0x22102011;		//��Ҫ���ŷ�
#endif


#endif


 

