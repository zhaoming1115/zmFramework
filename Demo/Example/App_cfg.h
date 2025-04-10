/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： App_cfg.h 
开发日期：2025-04-09 
文件功能：定义业务逻辑需要的配置数据。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __YW_APP_CFG_H_
#define __YW_APP_CFG_H_

//#include "FlashAddressFenPei.h"
//#include "YeWuConfig_GPIO_MZ.h"
#include "../App_cfg.h"

#define YW_PrintfErr(...)				SPrintf(0,__VA_ARGS__)
#define YW_PrintfMainInfo(...)			SPrintf(1,__VA_ARGS__)
#define YW_PrintfInfo(...)				SPrintf(2,__VA_ARGS__)
#define YW_PrintfAllInfo(...)			SPrintf(3,__VA_ARGS__)
#define YW_PrintfData(Level,...)		SPrintfData(Level,__VA_ARGS__)




#endif
