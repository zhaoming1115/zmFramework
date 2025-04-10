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
/*���ļ�����PLOOC������������ԭ��˵��*/
/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� zmQueueWriter.h 
�������ڣ�2025-03-28 
�ļ����ܣ���ӦC�ļ���ͷ�ļ�������C�ļ����⹫�����������͡��ӿں�����ȫ�ֱ�����

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/

 #ifndef __ZM_QUEUE_WRITER_H_
#define __ZM_QUEUE_WRITER_H_

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "../Sys_DataType.h"
#include "../Sys_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition 
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__

#include "zmQueue.h"


def_interface(i_QueueWriter_t)
	int (*UpdateWriteIndex)(c_Queue_t* Queue,unsigned short NewIndex);
end_def_interface(i_QueueWriter_t) /*do not remove this for forward compatibility */

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_QueueWriter_t* const zmQueueWriterManager;

/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
