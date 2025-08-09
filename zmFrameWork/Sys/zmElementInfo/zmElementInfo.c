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
�� �� ���� zmElementInfo.c 
�������ڣ�2025-04-09 
�ļ����ܣ�

��    ����

�����ӿڣ�

�ṩ�ӿڣ�
	

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/


/*============================ INCLUDES ======================================*/
#define __ElementInfo_CLASS_IMPLEMENT__

#include "./zmElementInfo.h"
#include "PAL/PAL.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef ASSERT
#   define ASSERT(...)    assert(__VA_ARGS__)
#endif

#undef this
#define this        (ptThis)

/*============================ TYPES =========================================*/
typedef struct
{
	const i_ElementInfoEvent_t* ElementInfoEvent;
}s_RunParm_t;

/*============================ LOCAL VARIABLES ===============================*/
static s_RunParm_t g_RunParm={0};

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
const s_ElementInfo_t* zmElementInfos=NULL;

/*============================ ˽�к��� ======================================*/
static int ElementInfo_Check(int MaxCount)
{
	if(g_RunParm.ElementInfoEvent->CHeckElement==NULL) return 0;
	typeof(g_RunParm.ElementInfoEvent->CHeckElement) CHeckElementFun=g_RunParm.ElementInfoEvent->CHeckElement;

	const s_ElementInfo_t* ElementInfo=zmElementInfos;
	for(int i=0;i<MaxCount;i++,ElementInfo++)
	{
		s_ElementInfo_t newInfo;
		memcpy(&newInfo,ElementInfo,sizeof(*ElementInfo));
		e_ElementInfoCHeckResult_t checkrst=CHeckElementFun(&newInfo);
		if(checkrst==Element_CHeckRst_InfoCHanged)
		{
			int count=3;
			int rst;
			do
			{
				rst=zmStorageManager->Write((unsigned int)&ElementInfo->ElementAddr,(const char*)&newInfo.ElementAddr,sizeof(newInfo.ElementAddr));
				if(rst>=0) break;
				Sys_PrintfError("д��Ԫ�ص�ַʱʧ�ܣ�ԭֵ=%08X����ֵ=%08X���쳣��=%d\n",ElementInfo->ElementAddr,newInfo.ElementAddr,rst);
			}while(--count>0);
			if(count<=0)
			{
				Sys_PrintfError("��γ�����Ȼ�޷�д��Ԫ��(%s)�ĵ�ַ\n",ElementInfo->Name);
			}
		}
	}
	while(zmStorageManager->Fulsh()){};
	return 0;
}


/*============================ IMPLEMENTATION ================================*/
/*! \brief the constructor of the class: ElementInfo */
static
int ElementInfo_init(unsigned int ElementInfoAddress, unsigned int RegionSize,const i_ElementInfoEvent_t* const ElementInfoEvent)
{
	zmElementInfos=(typeof(zmElementInfos))ElementInfoAddress;
	g_RunParm.ElementInfoEvent=ElementInfoEvent;
	if(g_RunParm.ElementInfoEvent)
	{
		ElementInfo_Check(RegionSize/sizeof(*zmElementInfos));
	}
    return 0;
}

static
int ElementInfo_ToString(const s_ElementInfo_t* ElementInfo,char* Buffer)
{
	static const char* const _TypeText[]={"�Զ������","�޷����ֽ�","�з����ֽ�","�޷��Ŷ�����","������",
										"�޷�������","����","�޷��ų�����","������","�����ȸ�����","˫���ȸ�����"};
	char* To=Buffer;
	To+=sprintf(To,"%s",ElementInfo->Name);
	if(Element_IsArray(ElementInfo))
	{
		To+=sprintf(To,"[%d]",Element_ArrayCount(ElementInfo));
	}

	if(ElementInfo->Type & DataType_StringFlag)
	{
		To+=sprintf(To,"=�ַ���,����<=%d",ElementInfo->MinValue);
	}
	else if(ElementInfo->Type>=DataType_ByteArrayBase)
	{
		To+=sprintf(To,"=�ֽڴ�,����<=%d",ElementInfo->MinValue);
	}
	else 
	{
		To+=sprintf(To,"=%s",_TypeText[ElementInfo->Type]);
		if(ElementInfo->Type==DataType_Fun)
		{
			f_Element_Fun_t fun=(f_Element_Fun_t)ElementInfo->ElementAddr;
			int len=fun(-1,To,NULL);
			if(len<=0)
			{
				strcpy(To,",�޲���");
				To+=sizeof(",�޲���")-1;
			}
			else
			{
				To+=len;
			}
		}
		else
		{
			*To++='[';
			To+=sprintf(To,g_NumberFormatList[ElementInfo->Type],Element_IsDoubleType(ElementInfo->Type)?*(long long*)&ElementInfo->DoubleMinValue:*(int*)&ElementInfo->MinValue);
			*To++=',';
			To+=sprintf(To,g_NumberFormatList[ElementInfo->Type],Element_IsDoubleType(ElementInfo->Type)?*(long long*)&ElementInfo->DoubleMaxValue:*(int*)&ElementInfo->MaxValue);
			*To++=']';
		}
	}
	*To=0;	
	return To-Buffer;
}

const i_ElementInfo_t* const zmElementInfoManager = &(const i_ElementInfo_t){
    .Init =             &ElementInfo_init,
    .ToString=ElementInfo_ToString
    /* other methods */
};


/* EOF */