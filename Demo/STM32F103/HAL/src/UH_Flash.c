/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： UH_Flash.c 
开发日期：2025-04-09 
文件功能：

描    述：

依赖接口：

提供接口：
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

#include "../Inc/UH_Flash.h"
#include "string.h"

#define FLASH_WAITETIME  					0xfffffff          //FLASH等待超时时间，循环计数
#define FLASH_MinProgramSizeLevel			1				//最小可编程单元大小等级，基2,如：0-1字节，1-2字节，2-四字节
#define FLASH_ProgramSizeLevel				1				//最佳可编程单元大小等级，基2,如：0-1字节，1-2字节，2-四字节
//static char g_TmpBuffer[ZHL_FLASH_PageSize];

#define FLASH_OB_RDP_LEVEL_0            ((uint8_t)0xA5)
#define FLASH_OB_RDP_LEVEL_1            ((uint8_t)0x00)

#define FLASH2							((FLASH_TypeDef*)(FLASH_R_BASE+(0x40*1)))

/*========================================== 私有函数 =======================================================*/
static FLASH_TypeDef* UH_Flash_Getflashr(unsigned int Address)
{
#ifdef ZHL_FLASH_UseExFlash
    return (Address>=ZHL_SPIFLASH_BASE)?SPIFLASH:FLASH;
#else
    return FLASH;
#endif
}


static bool UH_Flash_Lock(FLASH_TypeDef* flashr)
{
    /* Set the LOCK Bit to lock the FLASH Registers access */
    flashr->CR |= FLASH_CR_LOCK;
#if ZHL_FLASH_UseExFlash
	if(flashr==SPIFLASH)
	{
		SPIFLASH_EWFinished();
	}
#endif
    return true;
}

static bool UH_Flash_Unlock(FLASH_TypeDef* flashr)
{
    if( ZHL_GetBits(flashr->CR, FLASH_CR_LOCK) != 0)
    {
        /* Authorize the FLASH Registers access */
        flashr->KEYR=FLASH_KEY1;
        flashr->KEYR=FLASH_KEY2;
        /* Verify Flash is unlocked */
        if(ZHL_GetBits(flashr->CR, FLASH_CR_LOCK) != 0)
        {
            return false;
        }
    }
    return true;
}

inline static bool UH_Flash_IsLock(FLASH_TypeDef* flashr)
{
	return flashr->CR & FLASH_CR_LOCK;
}

static
bool UH_Flash_IsBusy_Inner(FLASH_TypeDef* flashr)
{
	bool BusyFlag=flashr->SR & FLASH_SR_BSY;
	if(!BusyFlag)
	{
		bool DoneFlag=flashr->SR & FLASH_SR_EOP;
		if(DoneFlag)
		{
			ZHL_SetBits(flashr->SR, FLASH_SR_EOP);		//清除操作完成标志	
			ZHL_ClrBits(flashr->CR, FLASH_CR_PG | FLASH_CR_PER | FLASH_CR_MER | FLASH_CR_OPTPG | FLASH_CR_OPTER );
			UH_Flash_Lock(flashr);
		}
	}
	return BusyFlag;
}

static int UH_Flash_WaitForLastOperation_Inner(FLASH_TypeDef* flashr)
{
    uint32_t tickstart = FLASH_WAITETIME;

    while(ZHL_GetBit(flashr->SR,FLASH_SR_BSY_Pos) != 0)
    {
        if(--tickstart<=0)
        {
           return -2;			//超时
        }
    }

    /* Check FLASH End of Operation flag  */
    if (ZHL_GetBit(flashr->SR,FLASH_SR_EOP_Pos) != 0)
    {
        /* Clear FLASH End of Operation pending bit */
        ZHL_GetBit(flashr->SR,FLASH_SR_EOP_Pos);
    }
    int err=ZHL_GetBits(flashr->SR,(FLASH_SR_WRPRTERR | FLASH_SR_WRPRTERR)) | ZHL_GetBits(flashr->OBR,FLASH_OBR_OPTERR);

    if(err != 0)
    {
        ZHL_ClrBits(flashr->SR,err);
        return -1;		//异常
    }

    /* If there is no error flag set */
    return 0;

}

#if __Storage_StateCHeckOnPAL
#define UH_Flash_WaitForLastOperation(__flashr)					0


#else
#define UH_Flash_WaitForLastOperation							UH_Flash_WaitForLastOperation_Inner 
#endif



/*========================================== 接口函数 =====================================================*/
static
int UH_Flash_Init(const c_StorageBlock_t* StorageBlock)
{
	return 0;
}

static
int UH_Flash_Read(const c_StorageBlock_t* StorageBlock,unsigned int Address,char* ToData,unsigned int ByteCount)
{
	memcpy(ToData,(const char*)Address,ByteCount);
	return ByteCount;
}

static
int UH_Flash_WriteUnit(const c_StorageBlock_t* StorageBlock,unsigned int Address, const int* FromData)
{
    FLASH_TypeDef* flashr=UH_Flash_Getflashr(Address);
    int status = UH_Flash_WaitForLastOperation(flashr);
	if(status==0)
	{
		UH_Flash_Unlock(flashr);
		ZHL_SetBits(flashr->CR, FLASH_CR_PG);
		const short* _FromData=(const short*)FromData;
		*(__IO short*)Address = *_FromData++;
		Address+=2;
		*(__IO short*)Address = *_FromData;
	}
	return 1;
}

static
int UH_Flash_EraseUnit(const c_StorageBlock_t* StorageBlock,unsigned int Address)
{
    FLASH_TypeDef* flashr=UH_Flash_Getflashr(Address);
	int status =UH_Flash_WaitForLastOperation(flashr);
    if(status == 0)
    {
		while (UH_Flash_Unlock(flashr)==false) {};
		ZHL_SetBits(flashr->CR, FLASH_CR_PER);
		flashr->AR=Address;
		ZHL_SetBits(flashr->CR, FLASH_CR_STRT);
		return 1<<StorageBlock->EraseUnitLevel;
        /* Flush the caches to be sure of the data consistency */
//    UH_Flash_FlushCaches();
    }
	return status;
}

static
int UH_Flash_ReadBitUnit(const c_StorageBlock_t* StorageBlock,unsigned int BitOffsetAddress, int* ToData)
{
//    FLASH_TypeDef* flashr=UH_Flash_Getflashr(StorageBlock);
	if(StorageBlock->ClearBitsLevel==3 && !(BitOffsetAddress & 7))
	{
		unsigned int Address=StorageBlock->BaseAddress+(BitOffsetAddress>>3);
		*(unsigned char*)ToData=*(const unsigned char*)Address;
		return 8;
	}
	else if(StorageBlock->ClearBitsLevel==4  && !(BitOffsetAddress & 0XF))
	{
		unsigned int Address=StorageBlock->BaseAddress+(BitOffsetAddress>>4);
		*(unsigned short*)ToData=*(const unsigned short*)Address;
		return 16;
	}
	return 0;
}

static
int UH_Flash_WriteBitUnit(const c_StorageBlock_t* StorageBlock,unsigned int BitOffsetAddress, const char* FromData)
{
	if(StorageBlock->ClearBitsLevel==3 && !(BitOffsetAddress & 7))
	{
		unsigned int Address=StorageBlock->BaseAddress+(BitOffsetAddress>>3);
		FLASH_TypeDef* flashr=UH_Flash_Getflashr(Address);
		if(*(const unsigned char*)Address!=0xff && *FromData!=0) return 0;
		int status = UH_Flash_WaitForLastOperation(flashr);
		if(status<0) return status;
		UH_Flash_Unlock(flashr);
		ZHL_SetBits(flashr->CR, FLASH_CR_PG);
		*(__IO char*)Address = *FromData;
		return 8;
	}
	else if(StorageBlock->ClearBitsLevel==4  && !(BitOffsetAddress & 0XF))
	{
		unsigned int Address=StorageBlock->BaseAddress+(BitOffsetAddress>>4);
		FLASH_TypeDef* flashr=UH_Flash_Getflashr(Address);
		unsigned short data=*(unsigned char*)FromData++;
		data|=(*(unsigned char*)FromData)<<8;
		if(*(const unsigned short*)Address!=0xff && data!=0) return 0;
		int status = UH_Flash_WaitForLastOperation(flashr);
		if(status<0) return status;
		UH_Flash_Unlock(flashr);
		ZHL_SetBits(flashr->CR, FLASH_CR_PG);
		*(__IO short*)Address =data;	
		return 16;
	}
	return 0;
}


static
bool UH_Flash_IsBusy(const c_StorageBlock_t* StorageBlock,unsigned int Address)
{
	return UH_Flash_IsBusy_Inner(UH_Flash_Getflashr(Address));
}

static bool UH_Flash_OB_Lock(void)
{
    ZHL_ClrBits(FLASH->CR, FLASH_CR_OPTWRE);
    return true;
}

static bool UH_Flash_OB_UnLock(void)
{
    if( ZHL_GetBits(FLASH->CR, FLASH_CR_OPTWRE) == 0)
    {
        /* Authorize the FLASH Registers access */
        FLASH->OPTKEYR=FLASH_OPTKEY1;
        FLASH->OPTKEYR=FLASH_OPTKEY2;
        /* Verify Flash is unlocked */
        if(ZHL_GetBits(FLASH->CR, FLASH_CR_OPTWRE) == 0)
        {
            return false;
        }
    }
    return true;
}

inline static bool UH_Flash_IsRDP(void)
{
	return ZHL_FLASH_EnableReadProtect;
//    return (ZHL_GetBits(FLASH->OBR, FLASH_OBR_RDPRT)==FLASH_OBR_RDPRT);
}

static int FLASH_OB_RDP_LevelConfig(bool Enable)
{
  int status = UH_Flash_WaitForLastOperation(FLASH);
  
  if(status == 0)
  { 
    
    ZHL_SetBits(FLASH->CR, FLASH_CR_OPTER);
    ZHL_SetBits(FLASH->CR, FLASH_CR_STRT);

    status = UH_Flash_WaitForLastOperation_Inner(FLASH);

    ZHL_ClrBits(FLASH->CR, FLASH_CR_OPTER);

    if(status == 0)
    {
      ZHL_SetBits(FLASH->CR, FLASH_CR_OPTPG);
      OB->RDP=(Enable)?FLASH_OB_RDP_LEVEL_1:FLASH_OB_RDP_LEVEL_0;
      
      status = UH_Flash_WaitForLastOperation_Inner(FLASH);
      
      ZHL_ClrBits(FLASH->CR, FLASH_CR_OPTPG);
    }
  }
  
  return status;
}

static int UH_Flash_OBErase(bool DisableRDP)
{

    bool rdptmp = (DisableRDP)?false:UH_Flash_IsRDP();

    int status = UH_Flash_WaitForLastOperation(FLASH);

    if(status == 0)
    {

        ZHL_SetBits(FLASH->CR, FLASH_CR_OPTER);
        ZHL_SetBits(FLASH->CR, FLASH_CR_STRT);

        status = UH_Flash_WaitForLastOperation_Inner(FLASH);

        ZHL_ClrBits(FLASH->CR, FLASH_CR_OPTER);

        if(status == 0)
        {
            status = FLASH_OB_RDP_LevelConfig(rdptmp);
        }
    }

    return status;
}

static unsigned int UH_Flash_MakeProtectValue(unsigned int Address,unsigned int Size)
{
    if(Size==0 || Address<ZHL_FLASH_BASE || Address>=ZHL_FLASH_BASE+ZHL_FLASH_SIZE) return 0;
    unsigned int Vaule=0;
    int i=(Address-ZHL_FLASH_BASE) >>ZHL_FLASH_EWProtectUnitLevel;
	
    int Len=Size+(Address & (ZHL_FLASH_EWProtectUnitSize-1));
    while(Len>0 && Address<(ZHL_FLASH_BASE+ZHL_FLASH_EWProtectSize))
    {
        Vaule|=(1<<i);
        i++;
        Address+=ZHL_FLASH_EWProtectUnitSize;
        Len-=ZHL_FLASH_EWProtectUnitSize;
    }
    return Vaule;
}


static int UH_Flash_WriteEWProtectValue(unsigned int ProtectValue,bool DisableRDP)
{
    int status = UH_Flash_WaitForLastOperation(FLASH);

	UH_Flash_Unlock(FLASH);
    if(status == 0)
    {
		UH_Flash_OB_UnLock();
        status = UH_Flash_OBErase(DisableRDP);
        if (status == 0)
        {
            ZHL_SetBits(FLASH->CR, FLASH_CR_OPTPG);
			unsigned char* Value=(unsigned char*)&ProtectValue;
			unsigned short* To=(unsigned short*)&OB->WRP0;

			for(int i=0;i<4;i++)
			{
				*To++=(unsigned short)*Value++;
                status = UH_Flash_WaitForLastOperation_Inner(FLASH);
				if(status) break;
			}
            ZHL_ClrBits(FLASH->CR, FLASH_CR_OPTPG);
		}
		UH_Flash_OB_Lock();
	}
	UH_Flash_Lock(FLASH);
    return status;
}

static
bool UH_Flash_DisableEWProtect(const c_StorageBlock_t* StorageBlock,unsigned int Address,unsigned int Size,bool RDPFlag)
{
	if(RDPFlag) FLASH_OB_RDP_LevelConfig(false);
    unsigned int ProtectValue=UH_Flash_MakeProtectValue(Address,Size);
    if(ProtectValue==0) return true;
/*如果要取消的写保护区没有设置写保护*/
	if((ProtectValue & FLASH->WRPR)==ProtectValue)
	{
		return true;
	}
	else
	{
    /*合并除能保护位*/
		ProtectValue|=FLASH->WRPR;
		int rst=UH_Flash_WriteEWProtectValue(ProtectValue,RDPFlag);
		return (rst<0)?false:true;
	}
}

static
bool UH_Flash_EnableEWProtect(const c_StorageBlock_t* StorageBlock,unsigned int Address,unsigned int Size)
{
    unsigned int ProtectValue=UH_Flash_MakeProtectValue(Address,Size);
    if(ProtectValue==0) return true;
	unsigned int OldValue=~FLASH->WRPR;
/*如果要取消的写保护区已经设置写保护*/
	if((ProtectValue & OldValue)==ProtectValue)
	{
		return true;
	}
	else
	{
    /*合并使能保护位*/
		ProtectValue=(unsigned int)(~(OldValue | ProtectValue));
		int rst=UH_Flash_WriteEWProtectValue(ProtectValue,false);
		return (rst<0)?false:true;
	}
	return false;
}

static
bool UH_Flash_IsEWProtect(const c_StorageBlock_t* StorageBlock,unsigned int Address,unsigned int Size)
{
    unsigned int ProtectValue=UH_Flash_MakeProtectValue(Address,Size);
	if(ProtectValue==0)
	{
		return false;
	}
	unsigned int OldValue=FLASH->WRPR;
	OldValue&=ProtectValue;
    return !OldValue;
}

static const i_StorageBlock_t g_InnerFlashManager={.Init=UH_Flash_Init,
													.Read=UH_Flash_Read,
													.EraseUnit=UH_Flash_EraseUnit,
													.IsBusy=UH_Flash_IsBusy,
													.WriteUnit=UH_Flash_WriteUnit,
#if Storage_UseBitRW
													.WriteBitUnit=UH_Flash_WriteBitUnit,
#endif
//													.ReadBitUnit=UH_Flash_ReadBitUnit,
													.IsEWProtect=UH_Flash_IsEWProtect,
#ifdef BootProgram
													.EnableProtect=NULL,
#else
													.EnableProtect=UH_Flash_EnableEWProtect,
#endif
													.DisableProtect=UH_Flash_DisableEWProtect};

const c_StorageBlock_t g_InnerFlash={.Manager=&g_InnerFlashManager,
											.BaseAddress=ZHL_FLASH_BASE,
											.Size=ZHL_FLASH_SIZE,
											.StorageType=Storage_Type_InnerFlash,
											.EraseValue=0xff,
											.WriteUnitLevel=2,
											.EraseUnitLevel=ZHL_FLASH_PageSizeLevel,
											.ClearBitsLevel=4,
											.ReadbufSizeLevel=Storage_NoSurportLevel,
											.MaxEraseTime=50};
