/*****************************************************************************
----------------------------------开发者信息---------------------------------
版权所有：赵明
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： HAL_Interface_StorageProtect.h 
开发日期：2024-05-18 
文件功能：相应C文件的头文件，定义C文件对外公开的数据类型、接口函数及全局变量。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __H_A_L__INTERFACE__STORAGE_PROTECT_H_
#define __H_A_L__INTERFACE__STORAGE_PROTECT_H_

/*****************************************************************************
* 函 数 名： UH_Flash_MakeProtectValue
* 函数功能： 生成擦写保护值
* 参    数： Address: 擦写保护的起始地址
**			 Size: 擦写保护区域大小
* 返    回：生成的保护值，每一位代表2个扇区，1有效
* 描    述：本函数生成的保护值仅限于 前 ZHL_FLASH_EWProtectSize 大小，大于该值的区域不启用保护

* 开 发 者：赵明 
* 日    期： 2023-06-12

* 修改历史：
** 1. 新创建

 *****************************************************************************/
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
  int status = UH_Flash_WaitForLastOperation(FLASH,(uint32_t)FLASH_WAITETIME);
  
  if(status == 0)
  { 
    
    ZHL_SetBits(FLASH->CR, FLASH_CR_OPTER);
    ZHL_SetBits(FLASH->CR, FLASH_CR_STRT);

    status = UH_Flash_WaitForLastOperation(FLASH,(uint32_t)FLASH_WAITETIME);

    ZHL_ClrBits(FLASH->CR, FLASH_CR_OPTER);

    if(status == 0)
    {
      ZHL_SetBits(FLASH->CR, FLASH_CR_OPTPG);
      OB->RDP=(Enable)?FLASH_OB_RDP_LEVEL_1:FLASH_OB_RDP_LEVEL_0;
      
      status = UH_Flash_WaitForLastOperation(FLASH,(uint32_t)FLASH_WAITETIME);
      
      ZHL_ClrBits(FLASH->CR, FLASH_CR_OPTPG);
    }
  }
  
  return status;
}

static int UH_Flash_OBErase(void)
{

    bool rdptmp = UH_Flash_IsRDP();

    int status = UH_Flash_WaitForLastOperation(FLASH,(uint32_t)FLASH_WAITETIME);

    if(status == 0)
    {

        ZHL_SetBits(FLASH->CR, FLASH_CR_OPTER);
        ZHL_SetBits(FLASH->CR, FLASH_CR_STRT);

        status = UH_Flash_WaitForLastOperation(FLASH,(uint32_t)FLASH_WAITETIME);

        ZHL_ClrBits(FLASH->CR, FLASH_CR_OPTER);

        if(status == 0)
        {
            status = FLASH_OB_RDP_LevelConfig(rdptmp);
        }
    }

    return status;
}

static int UH_Flash_WriteEWProtectValue(unsigned int ProtectValue)
{
    int status = UH_Flash_WaitForLastOperation(FLASH,(unsigned int)FLASH_WAITETIME);

	UH_Flash_Unlock(FLASH);
    if(status == 0)
    {
		UH_Flash_OB_UnLock();
        status = UH_Flash_OBErase();
        if (status == 0)
        {
            ZHL_SetBits(FLASH->CR, FLASH_CR_OPTPG);
			unsigned char* Value=(unsigned char*)&ProtectValue;
			unsigned short* To=(unsigned short*)&OB->WRP0;

			for(int i=0;i<4;i++)
			{
				*To++=(unsigned short)*Value++;
                status = UH_Flash_WaitForLastOperation(FLASH,(unsigned int)FLASH_WAITETIME);
				if(status) break;
			}
            ZHL_ClrBits(FLASH->CR, FLASH_CR_OPTPG);
		}
		UH_Flash_OB_Lock();
	}
	UH_Flash_Lock(FLASH);
    return status;
}

bool UH_Flash_DisableEWProtect(const c_StorageBlock_t* StorageBlock,unsigned int Address,unsigned int Size)
{
	return true;
}

bool UH_Flash_EnableEWProtect(const c_StorageBlock_t* StorageBlock,unsigned int Address,unsigned int Size)
{
    unsigned int ProtectValue=UH_Flash_MakeProtectValue(Address,Size);
    if(ProtectValue==0) return Flash_Protect_IsNeedState;
	unsigned int OldValue=~FLASH->WRPR;
/*如果要取消的写保护区已经设置写保护*/
	if((ProtectValue & OldValue)==ProtectValue)
	{
		return Flash_Protect_IsNeedState;
	}
	else
	{
    /*合并使能保护位*/
		ProtectValue=(unsigned int)(~(OldValue | ProtectValue));
		int rst=UH_Flash_WriteEWProtectValue(ProtectValue);
		return (rst<0)?Flash_Protect_Error:Flash_Protect_Sucess;
	}
	return false;
}

bool UH_Flash_IsEWProtect(const c_StorageBlock_t* StorageBlock,unsigned int Address,unsigned int Size)
{
	return false;
}


#endif


 

