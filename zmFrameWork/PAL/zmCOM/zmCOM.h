/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： zmCOM.h 
开发日期：2024-04-13 
文件功能：串行通信接口对象的对外接口头文件。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __ZM_C_O_M_H_
#define __ZM_C_O_M_H_

#include "zmSerialPort/zmSerialPort.h"
#include "zmNet/zmNet.h"
#include "zmI2C/zmI2C.h"
#include "zmCOM_cfg.h"

#define New_COM0()							({const c_com_t* SAFE_NAME(com)=NULL;\
											SAFE_NAME(com);})

#define New_COM4(__PortID,__Setting,__InitParm,__EventHandler)		(zmCOM_GetType(__PortID)==zmCOM_Type_SerialPort)?(&(New_SerialPort(__PortID,__Setting,__InitParm,__EventHandler)->use_as__c_com_t)): \
																	New_COM0()

#define New_COM3(__PortID,__Setting,__InitParm)		New_COM4(__PortID,__Setting,__InitParm,NULL)
#define New_COM2(__PortID,__Setting)				New_COM3(__PortID,__Setting,NULL)

#define New_COM(...)						FUN_EVAL(New_COM, __VA_ARGS__)(__VA_ARGS__)

#define ToString_COM_Setting3(__PortID,__StringBuffer,__Setting)	(zmCOM_GetType(__PortID)==zmCOM_Type_SerialPort)?ToString_SerialPort_Setting(__StringBuffer,__Setting): \
																	(zmCOM_GetType(__PortID)==zmCOM_Type_Net)?ToString_Net_Setting(__StringBuffer,__Setting): \
																	0

#define ToString_COM_Setting2(__PortID,__StringBuffer)				(zmCOM_GetType(__PortID)==zmCOM_Type_SerialPort)?ToString_SerialPort_Setting(__StringBuffer): \
																	(zmCOM_GetType(__PortID)==zmCOM_Type_Net)?ToString_Net_Setting(__StringBuffer): \
																	0


#define ToString_COM_Setting(...)									FUN_EVAL(ToString_COM_Setting, __VA_ARGS__)(__VA_ARGS__)

#define ParseString_COM_Setting(__PortID,__String,__Setting)		(zmCOM_GetType(__PortID)==zmCOM_Type_SerialPort)?ParseString_SerialPort_Setting(__String,__Setting): \
																	(zmCOM_GetType(__PortID)==zmCOM_Type_Net)?ParseString_Net_Setting(__String,__Setting): \
																	0
#endif


 

