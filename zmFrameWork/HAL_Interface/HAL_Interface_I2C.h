/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
��Ȩ���У�����
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� UH_Uart.h 
�������ڣ�2024-03-24 
�ļ����ܣ���ӦC�ļ���ͷ�ļ�������C�ļ����⹫�����������͡��ӿں�����ȫ�ֱ�����

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/

 #ifndef __HAL_INTERFACE_I2C_H_
#define __HAL_INTERFACE_I2C_H_
#include "./PAL/PAL_DataType.h"
#include <stdbool.h>

void UH_I2C_Init(int PortIndex,const s_com_Setting_t* Setting);
int UH_I2C_TransData(int PortIndex,const s_com_Setting_t* const I2CSetting,const char* Data,int Length);
void UH_I2C_DeInit(int PortIndex);
bool UH_I2C_IsOpen(int PortIndex);
bool UH_I2C_IsBusy(int PortIndex);
int UH_I2C_UpdateSetting(int PortIndex,const s_com_Setting_t* const NewPortConfig);
int UH_I2C_GetSetting(int PortIndex,s_com_Setting_t* GetTo);

	//Ӳ���ص�


/*****************************************************************************
* �� �� ���� UHC_I2C_Connected,UHC_I2C_DisConnected
* �������ܣ� ���ӡ��Ͽ����Ӻ�Ļص�����
* ��    ���� UartIndex: [����] -�˿ں�
* ��    �أ� ��
* ��    ���� ��Ҫ����CDC���⴮��

* �� �� �ߣ����� 
* ��    �ڣ� 2022-03-31

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
extern void UHC_I2C_Connected(int PortIndex);
extern void UHC_I2C_DisConnected(int PortIndex);

/*
	����˵����֡�ڷְ�������ϻص�����
	����˵����
	���������	
		UartIndex-���ڱ�ţ�
	���������
	**data-���ڴ洢��һ�����ݵ�ָ��
	*datalength-���ڴ洢��һ�����ݵ�����
	����ֵ��1-����������Ҫ���ͣ�0-��֡�Ѿ�������ϣ�����-�쳣
*/
extern e_I2CSendCallBackResult_t UHC_I2C_PacketSended(int PortIndex,char** data,int* datalength);		
/*
	����˵����֡������ϻص�����
	����˵����
	���������	
		UartIndex-���ڱ�ţ�
	����ֵ����
*/

extern void UHC_I2C_DataSended(int PortIndex);

/*
	����˵����һ֡�������ݽ��ջص�����
	����˵����
	���������	
		UartIndex-���ڱ�ţ�
	DataLength-���յ������ݳ���
	����ֵ����
*/
extern void UHC_I2C_DataReceived(int PortIndex,unsigned int DataLength);

extern void UHC_I2C_SettingCHanged(int PortIndex,const void* const Setting);
extern void UHC_I2C_Error(int PortIndex,e_I2CErrorType_t ErrorType);

#endif


 

