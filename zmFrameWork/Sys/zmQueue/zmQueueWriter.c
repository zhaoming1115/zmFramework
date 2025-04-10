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
/*本文件基于PLOOC开发，以上是原版说明*/
/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： zmQueueWriter.c 
开发日期：2025-03-28 
文件功能：实现队列写入者对象

描    述：

依赖接口：

提供接口：zmQueueWriterManager对象
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

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