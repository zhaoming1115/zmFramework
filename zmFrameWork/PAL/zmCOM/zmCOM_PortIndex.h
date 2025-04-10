/*****************************************************************************
----------------------------------开发者信息---------------------------------
开 发 者：赵明

----------------------------------文件信息------------------------------------
文 件 名： zmCOM_PortIndex.h 
开发日期：2024-06-29 
文件功能：串行通信端口的编号分配文件。

描    述：

-----------------------------------版本信息-----------------------------------
版    本：V1.0.0.0
版本说明：新创建

*******************************************************************************/

 #ifndef __ZM_C_O_M__PORT_INDEX_H_
#define __ZM_C_O_M__PORT_INDEX_H_

typedef enum
{
	zmCOM_Type_SerialPort,		//串口
	zmCOM_Type_SPI=1<<4,				//SPI
	zmCOM_Type_I2C=2<<4,				//SPI
	zmCOM_Type_CAN=3<<4,				//SPI
	zmCOM_Type_Net=4<<4,				//网络
//	zmCOM_Type_NetAdapter=3<<4,				//网络适配器
	zmCOM_Type_XuNiFlag=1<<7,				//虚拟化标志
	zmCOM_Type_Mak=7<<4,				//网络适配器
	
}e_zmComType_t;

#define zmCOM_GetType(__PortID)								(__PortID & zmCOM_Type_Mak)
#define zmCOM_IsXuNiPort(__PortID)							(__PortID & zmCOM_Type_XuNiFlag)
#define zmCOM_GetIndexInType(__PortID)						(__PortID & 0xf)

#define zmCOM_CreatePortID3(__IndexInType,__Type,__XuNiFlag)			(__Type | (__IndexInType & 0xf) | (zmCOM_Type_XuNiFlag & __XuNiFlag))
#define zmCOM_CreatePortID2(__IndexInType,__Type)						zmCOM_CreatePortID3(__IndexInType,__Type,0)
#define zmCOM_CreatePortID1(__IndexInType)								zmCOM_CreatePortID2(__IndexInType,zmCOM_Type_SerialPort)
#define zmCOM_CreatePortID(...)											FUN_EVAL(zmCOM_CreatePortID, __VA_ARGS__)(__VA_ARGS__)

#endif


 

