/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� PAL_Config.h 
�������ڣ�2024-03-25 
�ļ����ܣ�����������������Ҫ���������ݡ�

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/

 #ifndef __P_A_L__CONFIG_H_
#define __P_A_L__CONFIG_H_
#include <stdio.h>
#include "../App_cfg.h"

/*========================================================== GPIO =================================================================*/
#define GPIO_PrintError								printf
#define GPIO_None									0xff

/*========================================================== ����ͨ�ſ� =================================================================*/
#define COM_PrintfError								printf
#define COM_FrameSizeLevel_Default					8
#define COM_QueueFrameCount_Default					4

#define SerialPort_PrintfError						COM_PrintfError
#define SerialPort_FrameSizeLevel_Default			8
#define SerialPort_QueueFrameCount_Default			4

#define I2C_PrintfError								COM_PrintfError
#define I2C_FrameSizeLevel_Default					5
#define I2C_QueueFrameCount_Default					1

/*========================================================== �洢 =================================================================*/
#define Storage_PrintfError							printf

#endif


 

