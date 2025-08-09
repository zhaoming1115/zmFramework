/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： Test.c 
开发日期：2025-04-09 
文件功能：

描    述：

依赖接口：

提供接口：
	

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/
#include "Test.h"
#include "string.h"
#include "stddef.h"
#include "stdio.h"
#include <stdarg.h>
#include "Sys/Sys.h"
#include "PAL/PAL.h"

/*============================================================= 变量定义 ========================================================*/

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

/*============================================ 调试元素 ================================================================================*/

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

static const s_ElementInfo_t g_T_ElementInfos[] __AddressOfElementInfo={Append_ElementInfo("系统时间",T_RWSysTime),
																		Append_ElementInfo("软件版本",g_ZYWDefaultConfig.VSoft)};
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

/*============================================ 存储器对象 ================================================================================*/

static void Flash_BlockCHanging(unsigned int Address,int Length)
{
	__set_BASEPRI(0x10);	//屏蔽优先级等于或低于1的中断
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
#define __TestData						"存储器测试数据"
#define __RWFlashAddress				(64*KB+0X08000000)
	
	ZT_BEGIN();
	YW_PrintfInfo("存储器测试任务开始\n");
	while(1)
	{
		zmStorageManager->Write(__RWFlashAddress,__TestData,sizeof(__TestData));
		char buf[sizeof(__TestData)];
		zmStorageManager->Read(__RWFlashAddress,buf,sizeof(__TestData));
		if(memcmp(__TestData,buf,sizeof(__TestData))==0) break;
		YW_PrintfInfo("存储器写入失败，1秒后重试\n");
		ZT_Sleep(1000);
	}
	YW_PrintfInfo("存储器测试任务结束\n");
	zmThreadManager->RemoveThread(NULL);
	ZT_END();
}

/*============================================ 串口对象 ================================================================================*/
#define CKFastMode						1			//快速串口数据处理模式，使能后，直接提取串口缓存区的指针进行操作，省去了数据搬运的时间
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
			YW_PrintfMainInfo("串口接收到%d字节的数据\n",count);
			zmSerialPortManager->Write(&testUart->use_as__c_com_t,buf,count);
		}
		else
		{
			YW_PrintfMainInfo("提取串口数据异常\n");
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
			YW_PrintfErr("读取串口数据异常，异常码：%d\n",count);
			
		}
		else if(count>0)
		{
			YW_PrintfMainInfo("串口接收到%d字节的数据\n",count);
			zmSerialPortManager->Write(&testUart->use_as__c_com_t,buf,count);
		}
		else
		{
			YW_PrintfMainInfo("串口接收到0字节的数据\n");
		}
	}
	ZT_END();
}

#endif

/*============================================ PWM对象 ================================================================================*/
static int T_PWMProcess(void)
{
	c_PWM_t _pwm;
	const s_PWM_cfg_t _PWMConfig={.JingDu=100,.PinLv=1000};
	_pwm.CHannelIndex=PWM_CHANNEL_ID;
	zmPWMManager->Init(&_pwm,&_PWMConfig);	
	zmPWMManager->Start(&_pwm);
	zmPWMManager->SetMaiKuan(&_pwm,50);
	YW_PrintfMainInfo("PWM启动成功,占空比=50%%\n");
	zmThreadManager->RemoveThread(NULL);		//PWM已经启动成功，删除当前任务，避免再次启动
	return ZT_ENDED;
}

/*============================================ I2C对象 ================================================================================*/
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

		zmI2CManager->Write(&testI2C->use_as__c_com_t,__ACCMD,sizeof(__ACCMD));		//下发命令
		YW_PrintfInfo("I2C发送>>");
		YW_PrintfData(0,__ACCMD,4);
		ZT_WAIT_UNTIL(zmI2CManager->Read(&testI2C->use_as__c_com_t,(char*)ReadTo,7)!=COM_OPRST_PortIsBusy);	//等待命令下发完毕后开始读取数据
		StartTick_R=zmSys->GetmTick();
		while(zmI2CManager->ReceiveByteCount(&testI2C->use_as__c_com_t,0)<=0)
		{
			ZT_Sleep(10);
			if(zmSys->GetmTickSpan(StartTick_R)>=100)
			{	
				YW_PrintfInfo("I2C读取超时");
				ZT_TimeOut();
			}
		};

		zmI2CManager->Read(&testI2C->use_as__c_com_t,(char*)ReadTo,8);
		YW_PrintfInfo("I2C读取<<");
		YW_PrintfData(0,ReadTo,8);
		ZT_Sleep(10000);
	}
	ZT_END();
}

/*============================================ 主任务 ================================================================================*/
 
static int T_Process(void)
{
	static const char* const weekday[]={"日\0","一\0","二\0","三\0","四\0","五\0","六\0"};
//	static const JG_UpdateEventLX _UpdateEvent={T_UpdateEvent,NULL};
	static int Counter;
	s_DateTime_t DT;
#ifndef DebugMode
	zmSys->FeedDog();
#endif

	ZT_BEGIN();

	char buf[64];
	zmDateTime->SetDateTime(G_DefaultDateTime);
	YW_PrintfMainInfo("系统时间：%s\n",zmDateTime->ToString(NULL,buf));
	
	while(1)
	{
		zmGPIOManager->WritePin(LED_GPIO_ID,GPIO_Value_CHange);
		ZT_Sleep(500);		
	}
	ZT_END();
}

static void T_Start(void)
{
	zmThreadManager->CreateAddThread(T_Process,Thread_YXJ_CallByLunXun,true);		//主任务，测试GPIO
	zmThreadManager->CreateAddThread(T_FlashTestProcess,Thread_YXJ_CallByLunXun,true);		//FLASH测试任务
	zmThreadManager->CreateAddThread(T_UartProcess,Thread_YXJ_CallByLunXun,true);		//串口测试任务
	zmThreadManager->CreateAddThread(T_PWMProcess,Thread_YXJ_CallByLunXun,true);		//PWM测试任务
	zmThreadManager->CreateAddThread(T_I2CProcess_Master,Thread_YXJ_CallByLunXun,true);		//PWM测试任务
}

/*============================================ 任务开始 ================================================================================*/

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

