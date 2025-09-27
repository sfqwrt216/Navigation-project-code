/**
 * @file main.h
 * @brief Main header file for STM32 Navigation System
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "stm32f4xx_hal.h"

/* Exported types */

/* Exported constants */
#define LED_GPS_Pin GPIO_PIN_13
#define LED_GPS_GPIO_Port GPIOC
#define LED_STATUS_Pin GPIO_PIN_14
#define LED_STATUS_GPIO_Port GPIOC

/* Exported macro */

/* Exported functions prototypes */
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */