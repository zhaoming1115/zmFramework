/*本文件基于PLOOC定制，以下是原版说明*/
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


#ifndef __zmSPISyncMaster_H__
#define __zmSPISyncMaster_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>

#include "../zmSPIMasterBase.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

extern const i_SPIMaster_t g_zmSPISyncMasterManager;
#define zmSPISyncMasterManager	(&g_zmSPISyncMasterManager)

#define Append_SPISyncMasterNode(__PortIndex,__CSPin)     {.Manager= &zmSPISyncMasterManager->use_as__i_SPIBase_t.use_as__i_com_t, \
															.PortIndex=zmCOM_CreatePortID(__PortIndex,zmCOM_Type_SPI),\
															.CSPin=__CSPin}

#endif
/* EOF */