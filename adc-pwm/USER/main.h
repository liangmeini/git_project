/**
  ******************************************************************************
  * @file ADC2_TriggerConversion\main.h
  * @brief This file contains the external declaration of main.c file.
  * @author STMicroelectronics - MCD Application Team
  * @version V1.1.1
  * @date 06/05/2009
  ******************************************************************************
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  * 
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  * @image html logo.bmp
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__
#include "pwm.h"
#include "stm8s_clk.h"
#include "STM8S003F3.h"
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Evalboard I/Os configuration */
#define LEDS_PORT (GPIOC)
#define LED_PIN_3  (GPIO_PIN_3)
#define LED_PIN_5  (GPIO_PIN_5)
#define LED_PIN_6  (GPIO_PIN_6)
#define LED_PIN_7  (GPIO_PIN_7)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#endif /* __MAIN_H__ */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
