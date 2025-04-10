/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� HAL_Interface_PWM.h 
�������ڣ�2025-02-09 
�ļ����ܣ���ӦC�ļ���ͷ�ļ�������C�ļ����⹫�����������͡��ӿں�����ȫ�ֱ�����

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/

 #ifndef __H_A_L__INTERFACE__P_W_M_H_
#define __H_A_L__INTERFACE__P_W_M_H_

#include "./PAL/PAL_DataType.h"
#include <stdbool.h>

void UH_PWM_Init(unsigned int channelindex,unsigned int pinlv,unsigned int jingdu);
void UH_PWM_Start(unsigned int channelindex);
void UH_PWM_Stop(unsigned int channelindex);
void UH_PWM_SetMaiKuan(unsigned int channelindex,unsigned int maikuan);
unsigned int UH_PWM_GetMaiKuan(unsigned int channelindex);
unsigned int UH_PWM_GetJingDu(unsigned int channelindex);

#endif


 

