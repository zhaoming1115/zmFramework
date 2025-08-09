/*****************************************************************************
----------------------------------��������Ϣ---------------------------------
�� �� �ߣ�����

----------------------------------�ļ���Ϣ------------------------------------
�� �� ���� Test.c 
�������ڣ�2025-04-09 
�ļ����ܣ�

��    ����

�����ӿڣ�

�ṩ�ӿڣ�
	

-----------------------------------�汾��Ϣ-----------------------------------
��    ����V1.0.0.0
�汾˵�����´���

*******************************************************************************/
#include "Test.h"
#include "string.h"
#include "stddef.h"
#include "stdio.h"
#include <stdarg.h>
#include "Sys/Sys.h"
#include "PAL/PAL.h"

/*============================================================= �������� ========================================================*/

//typedef struct s_ZYWBaseConfig_t s_ZYWBaseConfig_t;

typedef struct
{
	char VSoft[24];
	s_zmSys_cfg_t MCUConfig;
}s_ZYWBaseConfig_t;

#ifdef DebugMode
static const s_ZYWBaseConfig_t g_ZYWDefaultConfig=  {{"1.0.0.0"},{0, 1}};
#else
static const s_ZYWBaseConfig_t g_ZYWDefaultConfig=  {{"1.0.0.0"},{2000, 1}};

#endif

/*============================================ ����Ԫ�� ================================================================================*/

static int T_RWSysTime(int ArrayIndex,const char* PortParm,const char** ErrDesc)
{
	char Key[32];
	if(ArrayIndex<0)
	{
		strcpy((char*)PortParm,DateTime_Format);
		return sizeof(DateTime_Format)-1;
	}
	else if(PortParm[0]==0)
	{
		char* To=(char*)PortParm;
		{
			zmDateTime->ToString(NULL,To);
			return strlen(To);
		}
	}
	else 
	{
		int rst=zmDateTime->ParseString(PortParm,NULL);
		return (rst<0)?rst:0;
	}	
	
}

static const s_ElementInfo_t g_T_ElementInfos[] __AddressOfElementInfo={Append_ElementInfo("ϵͳʱ��",T_RWSysTime),
																		Append_ElementInfo("����汾",g_ZYWDefaultConfig.VSoft)};
static const unsigned int g_ElementAddrs[]={(unsigned int)T_RWSysTime,(unsigned int)g_ZYWDefaultConfig.VSoft};


static e_ElementInfoCHeckResult_t T_CHeckElementInfo_My(s_ElementInfo_t* ElementInfo)
{
	int i=0;
	foreach(g_T_ElementInfos)
	{
		if(StringIsSame(_->Name,ElementInfo->Name,strlen(_->Name)))
		{
			if(ElementInfo->ElementAddr==g_ElementAddrs[i]) return Element_CHeckRst_OK;
			ElementInfo->ElementAddr=g_ElementAddrs[i];
			return Element_CHeckRst_InfoCHanged;;
		}
		i++;
	}
	return Element_CHeckRst_NoElement;
}

static	e_ElementInfoCHeckResult_t T_CHeckElementInfo(s_ElementInfo_t* ElementInfo)
{
	e_ElementInfoCHeckResult_t Result=T_CHeckElementInfo_My(ElementInfo);
	return Result;
}

/*============================================ �洢������ ================================================================================*/

static void Flash_BlockCHanging(unsigned int Address,int Length)
{
	__set_BASEPRI(0x10);	//�������ȼ����ڻ����1���ж�
}

static void Flash_BlockCHanged(unsigned int Address,int Length)
{
	__set_BASEPRI(0);
}

static void T_FlashInit(void)
{
	static const i_StorageEvent_t _FlashEvent={Flash_BlockCHanging,Flash_BlockCHanged};
	zmStorageManager->Init(&(s_Storage_cfg_t)Append_StorageConfig_Default(),&_FlashEvent);
}

static int T_FlashTestProcess(void)
{
#define __TestData						"�洢����������"
#define __RWFlashAddress				(64*KB+0X08000000)
	
	ZT_BEGIN();
	YW_PrintfInfo("�洢����������ʼ\n");
	while(1)
	{
		zmStorageManager->Write(__RWFlashAddress,__TestData,sizeof(__TestData));
		char buf[sizeof(__TestData)];
		zmStorageManager->Read(__RWFlashAddress,buf,sizeof(__TestData));
		if(memcmp(__TestData,buf,sizeof(__TestData))==0) break;
		YW_PrintfInfo("�洢��д��ʧ�ܣ�1�������\n");
		ZT_Sleep(1000);
	}
	YW_PrintfInfo("�洢�������������\n");
	zmThreadManager->RemoveThread(NULL);
	ZT_END();
}

/*============================================ ���ڶ��� ================================================================================*/
#define CKFastMode						1			//���ٴ������ݴ���ģʽ��ʹ�ܺ�ֱ����ȡ���ڻ�������ָ����в�����ʡȥ�����ݰ��˵�ʱ��
#if CKFastMode
static int T_UartProcess(void)
{
	static const s_com_Setting_t _TestUartSetting=Append_SerialPort_Setting(115200,SerialPort_ParityBit_N, SerialPort_StopBit_1);
	static const s_Queue_cfg_t _RTQueueConfig=Append_QueueConfig(8,2);
	s_com_init_t _ComInitParm={.SendQueueConfig=&_RTQueueConfig,.ReceiveQueueConfig=&_RTQueueConfig};
	static c_SerialPort_t* testUart=NULL;

	ZT_BEGIN();
	testUart=New_SerialPort(CK_COM_ID,&_TestUartSetting);
	zmSerialPortManager->Open(&testUart->use_as__c_com_t,NULL);

	while(1)
	{
		ZT_WAIT_UNTIL(zmSerialPortManager->ReceiveByteCount(&testUart->use_as__c_com_t,true));
		int count;
		char* buf=zmSerialPortManager->PickReceivedStream(&testUart->use_as__c_com_t,&count);
		if(buf)
		{
			YW_PrintfMainInfo("���ڽ��յ�%d�ֽڵ�����\n",count);
			zmSerialPortManager->Write(&testUart->use_as__c_com_t,buf,count);
		}
		else
		{
			YW_PrintfMainInfo("��ȡ���������쳣\n");
		}
	}
	ZT_END();
}

#else
static int T_UartProcess(void)
{
	static const s_com_Setting_t _TestUartSetting=Append_SerialPort_Setting(115200,SerialPort_ParityBit_N, SerialPort_StopBit_1);
	static const s_Queue_cfg_t _RTQueueConfig=Append_QueueConfig(8,2);
	s_com_init_t _ComInitParm={.SendQueueConfig=&_RTQueueConfig,.ReceiveQueueConfig=&_RTQueueConfig};
	static c_SerialPort_t* testUart=NULL;

	ZT_BEGIN();
	testUart=New_SerialPort(CK_COM_ID,&_TestUartSetting);
	zmSerialPortManager->Open(&testUart->use_as__c_com_t,NULL);

	while(1)
	{
		ZT_WAIT_UNTIL(zmSerialPortManager->ReceiveByteCount(&testUart->use_as__c_com_t,true));
		char buf[256];
		int count=zmSerialPortManager->Read(&testUart->use_as__c_com_t,buf,sizeof(buf));
		if(count<0)
		{
			YW_PrintfErr("��ȡ���������쳣���쳣�룺%d\n",count);
			
		}
		else if(count>0)
		{
			YW_PrintfMainInfo("���ڽ��յ�%d�ֽڵ�����\n",count);
			zmSerialPortManager->Write(&testUart->use_as__c_com_t,buf,count);
		}
		else
		{
			YW_PrintfMainInfo("���ڽ��յ�0�ֽڵ�����\n");
		}
	}
	ZT_END();
}

#endif

/*============================================ PWM���� ================================================================================*/
static int T_PWMProcess(void)
{
	c_PWM_t _pwm;
	const s_PWM_cfg_t _PWMConfig={.JingDu=100,.PinLv=1000};
	_pwm.CHannelIndex=PWM_CHANNEL_ID;
	zmPWMManager->Init(&_pwm,&_PWMConfig);	
	zmPWMManager->Start(&_pwm);
	zmPWMManager->SetMaiKuan(&_pwm,50);
	YW_PrintfMainInfo("PWM�����ɹ�,ռ�ձ�=50%%\n");
	zmThreadManager->RemoveThread(NULL);		//PWM�Ѿ������ɹ���ɾ����ǰ���񣬱����ٴ�����
	return ZT_ENDED;
}

/*============================================ I2C���� ================================================================================*/
static int T_I2CProcess_Master(void)
{
	static const s_com_Setting_t _TestI2CSetting=Append_I2CNode_Setting(0x38);
	static const s_Queue_cfg_t _RTQueueConfig=Append_QueueConfig(8,2);
	s_com_init_t _ComInitParm={.SendQueueConfig=&_RTQueueConfig,.ReceiveQueueConfig=&_RTQueueConfig};
	static c_I2CNode_t* testI2C=NULL;

	ZT_BEGIN();
	if(testI2C==NULL)
	{
		testI2C=New_I2CNode(I2C_Node_ID,&_TestI2CSetting);
		zmI2CManager->Open(&testI2C->use_as__c_com_t,NULL);
	}

	while(1)
	{
		static const char __ACCMD[]={0x70,0xac,0x33,0x00};
		static unsigned char ReadTo[16];
		static unsigned int StartTick_R;

		zmI2CManager->Write(&testI2C->use_as__c_com_t,__ACCMD,sizeof(__ACCMD));		//�·�����
		YW_PrintfInfo("I2C����>>");
		YW_PrintfData(0,__ACCMD,4);
		ZT_WAIT_UNTIL(zmI2CManager->Read(&testI2C->use_as__c_com_t,(char*)ReadTo,7)!=COM_OPRST_PortIsBusy);	//�ȴ������·���Ϻ�ʼ��ȡ����
		StartTick_R=zmSys->GetmTick();
		while(zmI2CManager->ReceiveByteCount(&testI2C->use_as__c_com_t,0)<=0)
		{
			ZT_Sleep(10);
			if(zmSys->GetmTickSpan(StartTick_R)>=100)
			{	
				YW_PrintfInfo("I2C��ȡ��ʱ");
				ZT_TimeOut();
			}
		};

		zmI2CManager->Read(&testI2C->use_as__c_com_t,(char*)ReadTo,8);
		YW_PrintfInfo("I2C��ȡ<<");
		YW_PrintfData(0,ReadTo,8);
		ZT_Sleep(10000);
	}
	ZT_END();
}

/*============================================ ������ ================================================================================*/
 
static int T_Process(void)
{
	static const char* const weekday[]={"��\0","һ\0","��\0","��\0","��\0","��\0","��\0"};
//	static const JG_UpdateEventLX _UpdateEvent={T_UpdateEvent,NULL};
	static int Counter;
	s_DateTime_t DT;
#ifndef DebugMode
	zmSys->FeedDog();
#endif

	ZT_BEGIN();

	char buf[64];
	zmDateTime->SetDateTime(G_DefaultDateTime);
	YW_PrintfMainInfo("ϵͳʱ�䣺%s\n",zmDateTime->ToString(NULL,buf));
	
	while(1)
	{
		zmGPIOManager->WritePin(LED_GPIO_ID,GPIO_Value_CHange);
		ZT_Sleep(500);		
	}
	ZT_END();
}

static void T_Start(void)
{
	zmThreadManager->CreateAddThread(T_Process,Thread_YXJ_CallByLunXun,true);		//�����񣬲���GPIO
	zmThreadManager->CreateAddThread(T_FlashTestProcess,Thread_YXJ_CallByLunXun,true);		//FLASH��������
	zmThreadManager->CreateAddThread(T_UartProcess,Thread_YXJ_CallByLunXun,true);		//���ڲ�������
	zmThreadManager->CreateAddThread(T_PWMProcess,Thread_YXJ_CallByLunXun,true);		//PWM��������
	zmThreadManager->CreateAddThread(T_I2CProcess_Master,Thread_YXJ_CallByLunXun,true);		//PWM��������
}

/*============================================ ����ʼ ================================================================================*/

static void T_HALInit(void)
{
	zmGPIOManager->Init(0);
	T_FlashInit();
	zmSys->Init(&g_ZYWDefaultConfig.MCUConfig,0);

}


static void T_ZuJianInit(void)
{
	static const s_Thread_cfg_t _ThreadConfig={16,8,1};
	static const i_ElementInfoEvent_t _ElementInfoEvent={T_CHeckElementInfo};

	zmThreadManager->Init(&_ThreadConfig,0);

	zmElementInfoManager->Init(Sys_StorageAddress_ElementInfo,Sys_StorageSize_ElementInfo,&_ElementInfoEvent);

}

void T_Enter(void)
{
	T_HALInit();
	zmSys->mDelay(100);
	T_ZuJianInit();

	T_Start();
	while(1)
	{
		zmThreadManager->Run();
	}

}

