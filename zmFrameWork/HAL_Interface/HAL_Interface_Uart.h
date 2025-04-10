/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� HAL_Interface_Uart.h 
�������ڣ�2025-03-26 
�ļ����ܣ������첽���ڵ�Ӳ���ӿڡ�

��    ����

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/
 #ifndef __HAL_INTERFACE_UART_H_
#define __HAL_INTERFACE_UART_H_
#include "./PAL/PAL_DataType.h"
#include <stdbool.h>

void UH_UART_Init(int UartIndex,const s_com_Setting_t* Setting);
int UH_UART_SendData(int UartIndex,const char* Data,int Length);
int UH_UART_SendChar(int UartIndex,const char data);
void UH_UART_EnableTCInterrupt(int UartIndex);
void UH_UART_DeInit(int UartIndex);
/*
	����˵������������
	����˵����
	���������
				UartIndex-���ڱ��
				*Data-���ݽ��յ�ַ
				Length-����������
	����ֵ��1-����������Ҫ���ͣ�0-��֡�Ѿ�������ϣ�����-�쳣
*/
void UH_UART_StartReceived(int UartIndex,char* Data,int Length);
bool UH_UART_IsOpen(int UartIndex);
bool UH_UART_IsSending(int UartIndex);
unsigned short UH_UART_GetRxOffsetAddr(int UartIndex);		//��ȡDMA���ջ���ָ�룬��δʹ��DMA�첽���գ��践��-1
unsigned short UH_UART_GetTxOffsetAddr(int UartIndex);	//��ȡDMAд�뻺��ָ�룬��δʹ��DMA�첽���գ��践��-1
int UH_UART_UpdateSetting(int UartNum,const s_com_Setting_t* const NewPortConfig);
int UH_UART_GetSetting(int UartNum,s_com_Setting_t* GetTo);

	//Ӳ���ص�


/*****************************************************************************
* �� �� ���� UHC_UART_Connected,UHC_UART_DisConnected
* �������ܣ� ���ӡ��Ͽ����Ӻ�Ļص�����
* ��    ���� UartIndex: [����] -�˿ں�
* ��    �أ� ��
* ��    ���� ��Ҫ����CDC���⴮��

* �� �� �ߣ����� 
* ��    �ڣ� 2022-03-31

* �޸���ʷ��
** 1. �´���

 *****************************************************************************/
extern void UHC_UART_Connected(int UartIndex);
extern void UHC_UART_DisConnected(int UartIndex);

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
extern e_UartSendCallBackResult_t UHC_UART_PacketSended(int UartIndex,char** data,int* datalength);		
/*
	����˵����֡������ϻص�����
	����˵����
	���������	
		UartIndex-���ڱ�ţ�
	����ֵ����
*/

extern void UHC_UART_DataSended(int UartIndex);

/*
	����˵����һ֡�������ݽ��ջص�����
	����˵����
	���������	
		UartIndex-���ڱ�ţ�
	DataLength-���յ������ݳ���
	����ֵ����
*/
extern void UHC_UART_DataReceived(int UartIndex,unsigned int DataLength,bool BufferFulledFlag);

extern void UHC_UART_SettingCHanged(int UartIndex,const void* const Setting);
extern void UHC_UART_Error(int UartIndex,e_SerialPortErrorType_t ErrorType);

#endif


 

