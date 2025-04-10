/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_UART4_Init(unsigned char PinGroupID);
void MX_USART1_UART_Init(unsigned char PinGroupID);
void MX_USART2_UART_Init(unsigned char PinGroupID);

/* USER CODE BEGIN Prototypes */
void MX_USART3_UART_Init(unsigned char PinGroupID);
void MX_UART5_Init(unsigned char PinGroupID);
void MX_USART6_UART_Init(unsigned char PinGroupID);
void MX_UART7_Init(unsigned char PinGroupID);
void MX_UART8_Init(unsigned char PinGroupID);
/* USER CODE END Prototypes */

#define IOMUX_MAKE_VALUE(reg_offset, bit_addr ,bit_num, bit_val) \
        (uint32_t)(((reg_offset) << 24) | ((bit_addr) << 16) | ((bit_num) << 8) | (bit_val))
#define USART6_GMUX                      IOMUX_MAKE_VALUE(0x34, 20, 4, 0x01) /*!< usart6_tx(pa4), usart6_rx(pa5) */
#define UART7_GMUX                       IOMUX_MAKE_VALUE(0x34, 24, 4, 0x01) /*!< uart7_tx(pb4),  uart7_rx(pb3) */
#define UART8_GMUX                       IOMUX_MAKE_VALUE(0x34, 28, 4, 0x01) /*!< uart8_tx(pc2),  uart8_rx(pc3) */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
