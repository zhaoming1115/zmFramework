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
/*本文件基于PLOOC定制，以上是原版说明*/
/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： zmStorage.c 
开发日期：2025-03-26 
文件功能：实现存储器管理器对象

描    述：存储器管理器对象特点如下：
1. 可同时管理多个存储器设备，无论是否相同的类型；
2. 支持清除操作，相当于让存储器的每个写入单元在一个擦除周期内，拥有了三种状态：擦除态、数据态、清除态，如果写入单元为1个bit除外；
3. 在擦除动作的前后会触发相应的事件通知，调用者可在事件中添加自己的逻辑，比如开关中断等；
4. 支持存储器的擦写保护操作；

依赖接口：HAL_Interface_Storage.h 文件中的接口

提供接口：
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/


/*============================ INCLUDES ======================================*/
#define __Storage_CLASS_IMPLEMENT__

#include "zmStorage.h"
#include "HAL_Interface/HAL_Interface_Storage.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "Sys/Sys.h"

/*============================ MACROS ========================================*/
#define	Storage_GetTmpBufferWithReadPage		(-1)			//获取临时缓存区并读取设定地址的一个整页

/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)    assert(__VA_ARGS__)
#endif

#undef this
#define this        		(ptThis)
#define thisblock      		this->StorageBlock
#define thisbuffer_w		this->WriteBuffer
#define thisblockManager  	thisblock->Manager
#define thisthread		  	(&this->Thread)

#define	class_MidData1(Address)			c_StorageBlockMidData_t* ptThis=Storage_GetBlockMidData(Address); \
										if(this==NULL) return Err_Code_ParmFail;

#define	class_MidData0()				c_StorageBlockMidData_t* ptThis=StorageBlockMidData

#define	class_MidData(...)				FUN_EVAL(class_MidData,__VA_ARGS__)(__VA_ARGS__)

//#define Storage_CanWrite()				({bool SafeName(WFlag)=(this->State==Storage_State_IDLE); \
//										if(!SafeName(WFlag)) { \
//											if(!thisblockManager->IsBusy(thisblock)) {\
//												this->State=Storage_State_IDLE; \
//												SafeName(WFlag)=true;}}\
//										SafeName(WFlag);})
//											
											
#define Storage_BeginWrite()			if(!(this->State==Storage_State_IDLE)) return Err_Code_Busy;\
										this->State=Storage_State_Busy											
										
#define Storage_EndWrite(__Address)			({ bool SafeName(busyFlag)=thisblockManager->IsBusy(thisblock,__Address); \
											if(!SafeName(busyFlag)) this->State=Storage_State_IDLE; \
											!SafeName(busyFlag);})


#define Storage_BeginErase()			Storage_BeginWrite()
											
#define Storage_EndErase(__Address)		Storage_EndWrite(__Address)

#define Storage_BeginRead()				Storage_BeginWrite()											

#define Storage_EndRead()				this->State=Storage_State_IDLE

/*============================ TYPES =========================================*/
typedef enum
{
	Storage_OpType_None,
	Storage_OpType_Read,
	Storage_OpType_Erase,
	Storage_OpType_Write,
	Storage_OpType_LockFlag=1<<2,
}e_StorageOperationType_t;

typedef struct
{
	unsigned int StorageAddress;
	unsigned short Size;
	unsigned short UsedBufferLength;
	unsigned char Data[] __Aligned(4);
}s_StorageBuffer_t;

typedef struct
{
	unsigned int StorageAddress;
	union
	{
		const char* Data;
		char SHortData[4];
	};
	unsigned short Length;
	unsigned char OpType;		//操作类型
	unsigned char EraseUnitCount;		//已经擦除的单元数量
	void (*FinishCallBack)(int OpResult,const char* Data,int Length);
}s_StorageOpInfo_t;

declare_class(c_StorageBlockMidData_t)

def_class(c_StorageBlockMidData_t,

    public_member(
        //!< place your public members here
		implement(c_ListItem_t);
		const c_StorageBlock_t* StorageBlock;
		s_ProtoThread_t Thread;
		e_StorageState_t State;
 		s_StorageBuffer_t* ReadBuffer;
 		s_StorageBuffer_t* WriteBuffer;
		s_StorageOpInfo_t* CurOpInfo;
   )

    private_member(
        //!< place your private members here
    )
    
    protected_member(
        //!< place your private members here
    )
)

end_def_class(c_StorageBlockMidData_t) /* do not remove this for forward compatibility  */

typedef struct
{
	const i_StorageEvent_t* Event;
	s_StorageOpInfo_t* EWInfoList;
	unsigned char EWInfoCount;
}s_StorageRunParm_t;

/*============================ LOCAL VARIABLES ===============================*/
static c_List_t g_StorageBlockMidDataList;
static s_StorageRunParm_t g_RunParm={0};

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

/*============================ 私有函数 =======================================*/
static int Storage_AddBlock(const c_StorageBlock_t* StorageBlock)
{
	if(!Storage_IsVaildLevel(StorageBlock->EraseUnitLevel))
	{
		Storage_PrintfError("无效的擦除单元等级\n");
		return Err_Code_ParmFail;
	}
	if(!Storage_IsVaildLevel(StorageBlock->WriteUnitLevel) && StorageBlock->WriteUnitLevel>StorageBlock->EraseUnitLevel)
	{
		Storage_PrintfError("无效的写入单元等级\n");
		return Err_Code_ParmFail;
	}
	unsigned int BufferSize=(1<<StorageBlock->EraseUnitLevel)+sizeof(s_StorageBuffer_t);
	if(Storage_IsVaildLevel(StorageBlock->ReadbufSizeLevel))
	{
		BufferSize+=(1<<StorageBlock->ReadbufSizeLevel)+sizeof(s_StorageBuffer_t);
	}	
	c_StorageBlockMidData_t* NewMidData=Sys_Malloc(sizeof(c_StorageBlockMidData_t)+BufferSize);
	if(NewMidData==NULL)
	{
		Storage_PrintfError("为存储块(地址=%08X)申请操作缓存失败\n",StorageBlock->BaseAddress);		
	}
	memset(NewMidData,0,sizeof(*NewMidData));
	with(NewMidData)
	{
		_->StorageBlock=StorageBlock;
		_->WriteBuffer=(s_StorageBuffer_t*)((char*)_+sizeof(*_));
		_->ReadBuffer=(s_StorageBuffer_t*)((char*)_->WriteBuffer+((1<<StorageBlock->EraseUnitLevel)+sizeof(s_StorageBuffer_t)));
		_->State=Storage_State_IDLE;
		ZT_INIT(&_->Thread);
		zmListManager->Add(&g_StorageBlockMidDataList,&_->use_as__c_ListItem_t);
	}
	return StorageBlock->Manager->Init(StorageBlock);
}

static c_StorageBlockMidData_t* Storage_GetBlockMidData(unsigned int Address)
{
	c_ListItem_t* Item=zmListManager->First(&g_StorageBlockMidDataList);
	while(Item)
	{
		this_from_implenment(Item,c_ListItem_t,c_StorageBlockMidData_t);\
		if(thisblock->BaseAddress<=Address && Address<thisblock->BaseAddress+thisblock->Size) return this;
		Item=zmListManager->Next(&g_StorageBlockMidDataList,Item);
	}
	return NULL;
}

static int Storage_Clear_Inner(c_StorageBlockMidData_t* StorageBlockMidData,unsigned int Address,unsigned int ByteLength)
{
	class_MidData();
	Storage_BeginWrite();
	
	Storage_EndWrite(Address);
	return 0;
}

static int Storage_WriteDataToBuffer(c_StorageBlockMidData_t* StorageBlockMidData,unsigned int Address,const char* FromData, unsigned int WriteCount)
{
	class_MidData();
	int rst=0;
	unsigned int EndAddr=thisbuffer_w->StorageAddress+thisbuffer_w->Size;
	if(Address>=thisbuffer_w->StorageAddress && Address<EndAddr)
	{
		int remsize=EndAddr-Address;
		if(remsize<WriteCount)
		{
			WriteCount=remsize;
		}
		Storage_BeginWrite();
		memcpy(thisbuffer_w->Data+Address-thisbuffer_w->StorageAddress,FromData,WriteCount);
		Storage_EndWrite(Address);
		rst=WriteCount;
	}
	return rst;
}

static int Storage_WriteDataToBlock(c_StorageBlockMidData_t* StorageBlockMidData,unsigned int Address,const int* FromData, int WordCount)
{
	class_MidData();
	Storage_BeginWrite();
	unsigned int WordsInUnit=1<<(thisblock->WriteUnitLevel-2);
	while(WordCount>0)
	{
		thisblockManager->WriteUnit(thisblock,Address,FromData);
		FromData+=WordsInUnit;
		Address+=WordsInUnit*4;
		WordCount-=WordsInUnit;
	}
	while(!Storage_EndWrite(Address));
	return 0;
}

static int Storage_WriteDataToBlockAsBits(c_StorageBlockMidData_t* StorageBlockMidData,unsigned int Address,const char* FromData, int ByteCount)
{
	class_MidData();
	Storage_BeginWrite();
	unsigned int BitOffsetAddress=(Address-thisblock->BaseAddress)<<3;
	unsigned int BitCount=ByteCount<<3;
	
	while(BitCount>0)
	{
//		int readbuf[((1<<thisblock->ClearBitsLevel)+31)>>5];
//		int rst=thisblockManager->ReadBitUnit(thisblock,BitOffsetAddress,readbuf);
//		if(rst<=0) break;
//		
		int rst=thisblockManager->WriteBitUnit(thisblock,BitOffsetAddress,FromData);
		if(rst<=0) break;
		BitCount-=rst;
		BitOffsetAddress+=rst;
		FromData+=rst>>3;
	}
	while(!Storage_EndWrite(Address));	
	return ByteCount-(BitCount>>3);
}

static int Storage_CHeck(c_StorageBlockMidData_t* StorageBlockMidData,unsigned int WriteAddress,const char* SrcData,int Length)
{
	class_MidData();
	if(Storage_IsWithSysLine(thisblock->StorageType))
	{
		if(memcmp((const char*)WriteAddress,SrcData,Length))
		{
			return Err_Code_Storage_WriteFail;
		}
	}
	else
	{
		char buffer[256];
		int rlen=sizeof(buffer);
		while(Length>0)
		{
			if(rlen>Length) rlen=Length;
			thisblockManager->Read(thisblock,WriteAddress,buffer,rlen);
			if(memcmp((const char*)buffer,SrcData,Length))
			{
				return Err_Code_Storage_WriteFail;
			}
			WriteAddress+=rlen;
			Length-=rlen;
		}
	}
	return 0;
}

static int Storage_Flush_Inner(c_StorageBlockMidData_t* StorageBlockMidData)
{
	class_MidData();
	if(g_RunParm.Event && g_RunParm.Event->BlockErasing) g_RunParm.Event->BlockErasing(thisbuffer_w->StorageAddress,thisbuffer_w->Size);
	Storage_BeginErase();
	thisblockManager->EraseUnit(thisblock,thisbuffer_w->StorageAddress);
	while(!Storage_EndErase(thisbuffer_w->StorageAddress));
	int rst=Storage_WriteDataToBlock(this,thisbuffer_w->StorageAddress,(const int*)thisbuffer_w->Data,thisbuffer_w->Size>>2);
	if(rst>=0) rst=Storage_CHeck(this,thisbuffer_w->StorageAddress,(char*)thisbuffer_w->Data,thisbuffer_w->Size);
	thisbuffer_w->StorageAddress=0;
	return 0;
}

static int Storage_FlushProcess(c_StorageBlockMidData_t* StorageBlockMidData)
{
	class_MidData();
	ZT_BEGIN(&this->Thread);
	if(thisbuffer_w->StorageAddress)
	{
		if(g_RunParm.Event && g_RunParm.Event->BlockErasing) g_RunParm.Event->BlockErasing(thisbuffer_w->StorageAddress,thisbuffer_w->Size);
		Storage_BeginErase();
		thisblockManager->EraseUnit(thisblock,thisbuffer_w->StorageAddress);
		ZT_WAIT_UNTIL(&this->Thread, Storage_EndErase(thisbuffer_w->StorageAddress));
		ZT_WAIT_UNTIL(Storage_WriteDataToBlock(this,thisbuffer_w->StorageAddress,(const int*)thisbuffer_w->Data,thisbuffer_w->Size>>2)>=0);
		thisbuffer_w->StorageAddress=0;
		if(g_RunParm.Event && g_RunParm.Event->BlockErased) g_RunParm.Event->BlockErased(thisbuffer_w->StorageAddress,thisbuffer_w->Size);
	}
	ZT_END(&this->Thread);
}

static unsigned int Storage_IsNeedErase_Alighed(const c_StorageBlock_t* StorageBlock,unsigned int Address,unsigned int EndAddress)
{
	int EraseValue=(int)StorageBlock->EraseValue;
	unsigned int PageHeaderMak=((1<<StorageBlock->EraseUnitLevel)-1);
	if(Storage_IsWithSysLine(StorageBlock->StorageType))
	{
		while (Address<EndAddress)
		{
			if (*(int*)Address!=EraseValue)
			{		
				return Address & ~PageHeaderMak;
			}
			Address+=4;
		}
	}
	else
	{
		#define BufLength				256

		int _buf[BufLength>>2];
		while(Address<EndAddress)
		{
			int len=EndAddress-Address;
			if(len>BufLength) len=BufLength;
			StorageBlock->Manager->Read(StorageBlock,Address,(char*)_buf,len);
			for(int i=0;i<len>>2;i++)
			{
				if (_buf[i]!=EraseValue)
				{		
				return Address & ~PageHeaderMak;
				}
			}
			Address+=BufLength;
		}
	}
	return 0;
}

static int Storage_IsNeedErase_Inner(c_StorageBlockMidData_t* StorageBlockMidData,unsigned int* Address,unsigned int ByteLength,char* EraseValue)
{
	class_MidData();
	char __EraseValue=thisblock->EraseValue;
	if(!(__EraseValue==0 || __EraseValue==-1)) return 0;
	unsigned int Addr=*Address;
	if(EraseValue) *EraseValue=__EraseValue;
	if (ByteLength)
	{
		int Value=(int)__EraseValue;
		unsigned int EndAddress=Addr+ByteLength;

		unsigned int ProgramUnitMak=(1<<thisblock->WriteUnitLevel)-1;	//写入单元所占的位掩码

		Addr&=~ProgramUnitMak;		//写入地址进行单元对齐
		EndAddress+=ProgramUnitMak;
		EndAddress&=~ProgramUnitMak;	//终止地址向上对齐到写入单元
		
		if((Addr=Storage_IsNeedErase_Alighed(thisblock,Addr,EndAddress)))
		{
			*Address=Addr;
			return EndAddress-Addr;
		}
	}
	return 0;
}

static int Storage_ReadInner(c_StorageBlockMidData_t* StorageBlockMidData,unsigned int Address,char* To,int ReadLength)
{
	class_MidData();
	Storage_BeginRead();
	int rst= thisblockManager->Read(thisblock,Address,To,ReadLength);
	Storage_EndRead();
	return rst;
}

static int* Storage_GetTmpBuffer(c_StorageBlockMidData_t* StorageBlockMidData,unsigned int Address,int ReadLength)
{
	class_MidData();
	if (ReadLength)	
	{
		unsigned short PageSize=1<<thisblock->EraseUnitLevel;
		if(ReadLength==Storage_GetTmpBufferWithReadPage)
		{
			ReadLength=PageSize;
			Address&=~(ReadLength-1);
		}
		else if(ReadLength>PageSize)
		{
			ReadLength=PageSize;
		}
		if (Address+ReadLength>=thisblock->BaseAddress+thisblock->Size)
		{
			ReadLength=thisblock->BaseAddress+thisblock->Size-Address;
		}
		int rst=Storage_ReadInner(this,Address,(char*)thisbuffer_w->Data,ReadLength);
		if(rst<0) return NULL;
	}
	return (int*)thisbuffer_w->Data;
}

static s_StorageOpInfo_t* Storage_AskInfoItem(e_StorageOperationType_t OpType)
{
	foreachitem(g_RunParm.EWInfoList,g_RunParm.EWInfoCount)
	{
		if(_->OpType==Storage_OpType_None)
		{
			_->OpType=OpType;
			return _;
		}
	}
	return NULL;
}

inline static int Storage_ReleaseInfoItem(s_StorageOpInfo_t* item)
{
	item->OpType=NULL;
	item->StorageAddress=0;
	return 0;
}

inline static s_StorageOpInfo_t* Storage_PickWaitOpItem(unsigned int StorageAddress,unsigned int Size)
{
	unsigned int EndAddress=StorageAddress+Size;
	foreachitem(g_RunParm.EWInfoList,g_RunParm.EWInfoCount)
	{
		if(!(_->OpType & Storage_OpType_LockFlag) && _->StorageAddress>=StorageAddress && _->StorageAddress<EndAddress)
		{
			_->OpType|=Storage_OpType_LockFlag;
			return _;
		}
	}
	return NULL;
}

inline static s_StorageOpInfo_t* Storage_FindOpingItem(void)
{
	foreachitem(g_RunParm.EWInfoList,g_RunParm.EWInfoCount)
	{
		if(_->OpType & Storage_OpType_LockFlag)
		{
			return _;
		}
	}
	return NULL;
}

inline static void Storage_OpFinished(int Result)
{
	foreachitem(g_RunParm.EWInfoList,g_RunParm.EWInfoCount)
	{
		if(_->OpType & Storage_OpType_LockFlag)
		{
			if(_->FinishCallBack) _->FinishCallBack(Result,(_->OpType==Storage_OpType_Read)?_->Data:(const char*)_->StorageAddress,_->Length);
			Storage_ReleaseInfoItem(_);
		}
	}
}

/*============================ IMPLEMENTATION ================================*/

static int Storage_Init(const s_Storage_cfg_t* const Config,const i_StorageEvent_t* const Event)
{
	g_RunParm.Event=Event;
	g_RunParm.EWInfoCount=(Config==NULL || Config->EWInfoCount<=0)?1:0;
	g_RunParm.EWInfoList=Sys_Malloc(g_RunParm.EWInfoCount * sizeof(*g_RunParm.EWInfoList));
	if(g_RunParm.EWInfoList==NULL)
	{
		Storage_PrintfError("申请操作项缓存失败\n");
		return Err_Code_AskMemoryFail;
	}
	memset(g_RunParm.EWInfoList,0,g_RunParm.EWInfoCount * sizeof(*g_RunParm.EWInfoList));
	Storage_AddDevices(Storage_AddBlock);	
	return 0;
}

static int Storage_Read(unsigned int Address,char* ToData,unsigned int Length)
{
	class_MidData(Address);
	if(ToData)
	{
		return Storage_ReadInner(this,Address,ToData,Length);
	}
	else if(this->ReadBuffer)
	{
		int rst=Storage_ReadInner(this,Address,ToData,Length);
		if(rst<0) return NULL;
		this->ReadBuffer->StorageAddress=Address;
		this->ReadBuffer->Size=Length;
		return (int)this->ReadBuffer->Data;
	}
	else
	{
		return 0;
	}
}

inline static int Storage_Erase(unsigned int StartAddress,unsigned int Length)
{
	int __len=Length;
	int rst=0;
	unsigned int EndAddress=StartAddress+Length;
	
	while(StartAddress<EndAddress)
	{
		class_MidData(StartAddress);
		Storage_BeginErase();
		int size=thisblockManager->EraseUnit(thisblock,StartAddress);
		while(!Storage_EndErase(StartAddress));
		if(size<=0)
		{		
			return Err_Code_Storage_EraseFail;			
		}
		__len-=size;
		rst+=size;
		StartAddress=(StartAddress+size) & ~(size-1);
	}
	return rst;
}

static int Storage_IsNeedErase(unsigned int* Address,unsigned int ByteLength,char* EraseValue)
{
	unsigned int StartAddress=*Address;
	class_MidData(StartAddress);
	return Storage_IsNeedErase_Inner(this,Address,ByteLength,EraseValue);
}

static int Storage_Flush(void)
{
	int busyFlag=0;
	c_ListItem_t* Item=zmListManager->First(&g_StorageBlockMidDataList);
	while(Item)
	{
		this_from_implenment(Item,c_ListItem_t,c_StorageBlockMidData_t);
		if(Storage_FlushProcess(this)!=ZT_ENDED) 
		{
			busyFlag++;
		}
		Item=zmListManager->Next(&g_StorageBlockMidDataList,Item);
	}
	return busyFlag;
}

/**
  * 函数功能: 从指定地址开始写入指定长度的数据
  * 输入参数: Address:相对于存储区起始地址的起始偏移地址
  *           pBuffer:数据指针
  *           ByteLength:写入字节数
  * 返 回 值: 成功写入的字节数,-1表示地址异常,-2表示写入未成功
  * 说    明：无
  */
static int Storage_Write(unsigned int Address, const char* FromData, unsigned int ByteLength)
{
#ifdef Storage_WriteAddressForFlush
	if(Address==Storage_WriteAddressForFlush) return Storage_Flush();
#endif
	class_MidData(Address);
		
	if (FromData==NULL)
	{
		return Storage_Clear_Inner(this,Address,ByteLength);
	}
	int __len=ByteLength;
	if(__len<(1<<thisblock->WriteUnitLevel) && !(Address>=thisbuffer_w->StorageAddress && Address+__len<thisbuffer_w->StorageAddress+thisbuffer_w->Size))
	{
		int wlen=Storage_WriteDataToBlockAsBits(this,Address,FromData,ByteLength);
		FromData+=wlen;
		__len-=wlen;		
	}
	unsigned int WriteAddr;
	int WriteCount,tmp;
	
	int PageSize=1<<thisblock->EraseUnitLevel;
	unsigned int ProgramUnitMak=(1<<thisblock->WriteUnitLevel)-1;
	const char* SrcData=FromData;
	const char* DstData=(const char*)Address;

	while(__len>0) 
	{
		WriteCount=PageSize-(Address&(PageSize-1));		//将写入数量限制到页尾
		if (WriteCount>__len)
		{
			WriteCount=__len;
		}
		
/*若写入区间有一部分位于缓存区中，则先将数据写入缓存区*/
		if(thisbuffer_w->StorageAddress)
		{
			tmp=Storage_WriteDataToBuffer(this,Address,FromData,WriteCount);
			if(tmp>0)
			{
				WriteCount=tmp;
				goto Next;
			}
			else if(tmp==0)
			{
				tmp= Storage_Flush_Inner(this);
				if (tmp<0)
				{
					Storage_PrintfError("写存储(地址=%08X,长度=%d)时将缓存区的数据刷入存储区(地址=%08X)时出现异常(%d)\n",Address,WriteCount,thisbuffer_w->StorageAddress,tmp);
					return tmp;
				}
			}
			else
			{
				Storage_PrintfError("写存储(地址=%08X,长度=%d)时将数据写入缓存区时出现异常(%d)\n",Address,WriteCount,tmp);
				return tmp;
			}
		}

		/*
		先判断要写入的部分是否需要擦除，若需要擦除，
		则使用读改写模式，操作字节数量为临时缓存区的大小，
		否则使用单元对齐的方式直接写入存储器
		Flash_IsNeedErase函数自动将WriteAddr对齐到页首地址
		*/
			//如果写入地址、源数据地址和写入数量有一个不是单元对齐的，则进行单元对齐
		int WRCount;
		if ((Address & ProgramUnitMak) || (WriteCount & ProgramUnitMak) )
		{
			WriteAddr=Address & ~ProgramUnitMak;		//将地址进行单元对齐
			WRCount=(WriteCount+Address-WriteAddr+ProgramUnitMak)&~ProgramUnitMak;
//			__TmpBuffer=(char*)Storage_GetTmpBuffer(this,WriteAddr,WRCount);//
		}
		else
		{
			WriteAddr=Address;
			WRCount=WriteCount;
//			__TmpBuffer=NULL;
		}
		unsigned int Addr=WriteAddr;
		if (Storage_IsNeedErase_Inner(this,&Addr,WRCount,NULL))
		{
			char* __TmpBuffer=(char*)Storage_GetTmpBuffer(this,WriteAddr,Storage_GetTmpBufferWithReadPage);//
			memcpy(__TmpBuffer+(Address-Addr),FromData,WriteCount);
			thisbuffer_w->Size=PageSize;
			thisbuffer_w->StorageAddress=Addr;
		}
		else 
		{
			if((unsigned int)FromData & 0X3)
			{
				int buffer[64];
				int remlen=WRCount;
				const char* _FromData=FromData;
				for(unsigned int _Addr=WriteAddr;_Addr<WriteAddr+WRCount;_Addr+=sizeof(buffer),_FromData+=sizeof(buffer),remlen-=sizeof(buffer))
				{
					int wlen=MIN(remlen,sizeof(buffer));
					memcpy(buffer,_FromData,wlen);
					Storage_WriteDataToBlock(this,_Addr,buffer,wlen>>2);				
				}
			}
			else
			{
				Storage_WriteDataToBlock(this,WriteAddr,(const int*)FromData,WRCount>>2);
			}
			int rst=Storage_CHeck(this,WriteAddr,FromData,WRCount);
			if(rst<0) return rst;
		}
Next:
		Address+=WriteCount;
		FromData+=WriteCount;
		__len-=WriteCount;
	}

//	if (__TmpBuffer)
//	{
//		Flash_ReleaseTmpBuffer();
//	}
	return ByteLength;
}

static
e_StorageState_t Storage_GetState(unsigned int Address)
{
	class_MidData(Address);
	return this->State;
}

static
bool Storage_DisableEWProtect(unsigned int Address,unsigned int Size,bool RDPFlag)
{
	class_MidData(Address);
	return thisblockManager->DisableProtect(thisblock,Address,Size,RDPFlag);
}

static
bool Storage_EnableEWProtect(unsigned int Address,unsigned int Size)
{
	class_MidData(Address);
	return thisblockManager->EnableProtect(thisblock,Address,Size);
}

static
bool Storage_IsEWProtect(unsigned int Address,unsigned int Size)
{
	class_MidData(Address);
	return thisblockManager->IsEWProtect(thisblock,Address,Size);
}


const i_Storage_t* const zmStorageManager = &(const i_Storage_t){
    .Init=Storage_Init,
	.Read=Storage_Read,
	.Erase=Storage_Erase,	
	.Write=Storage_Write,
	.Fulsh=Storage_Flush,
	.State=Storage_GetState,
	.IsNeedErase=Storage_IsNeedErase,
	.DisableProtect=Storage_DisableEWProtect,
	.EnableProtect=Storage_EnableEWProtect,
	.IsEWProtect=Storage_IsEWProtect
    /* other methods */
};


/* EOF */