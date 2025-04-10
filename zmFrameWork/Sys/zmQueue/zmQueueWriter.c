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
�� �� ���� zmQueueWriter.c 
�������ڣ�2025-03-28 
�ļ����ܣ�ʵ�ֶ���д���߶���

��    ����

�����ӿڣ�

�ṩ�ӿڣ�zmQueueWriterManager����
	

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/


/*============================ INCLUDES ======================================*/
#define __Queue_CLASS_IMPLEMENT__

#include "zmQueueWriter.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)    assert(__VA_ARGS__)
#endif

#undef this
#define this        (ptThis)

/*============================ TYPES =========================================*/

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

/*============================ IMPLEMENTATION ================================*/

/*! \brief the constructor of the class: Queue */


static int QueueWriter_UpdateWriteIndex(c_Queue_t* Queue,unsigned short ByteIndex)
{
	typeof(Queue->Config)* cfg=&Queue->Config;
	unsigned short ElementSize=Queue_GetElementSize();
	int DataLen=ByteIndex-Queue->WriteIndex*ElementSize;
	if(DataLen==0) return ByteIndex;
	if(DataLen<0) DataLen+=ElementSize*cfg->ElementCount;
	const i_Queue_t* Manager=Queue->Manager;
	int rst;
	do
	{
		rst=Manager->Enqueue(Queue,NULL,DataLen);
		if(rst==Queue_OPRST_NoEnoughBuffer) Manager->Dequeue(Queue,NULL,Queue_DataLength_OneFrame);
	}while(rst<0);
	return ByteIndex;
}

const i_QueueWriter_t* const zmQueueWriterManager = &(const i_QueueWriter_t){
    .UpdateWriteIndex =  &QueueWriter_UpdateWriteIndex,
    /* other methods */
};


/* EOF */