/**
 * Navigation System - STM32 Main Application
 * Course Project Implementation
 * 
 * This file contains the main application logic for the STM32-based
 * navigation system that interfaces with GPS modules and IMU sensors.
 */

#include "main.h"
#include "gps_module.h"
#include "imu_module.h"
#include "navigation.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

/* Private variables */
UART_HandleTypeDef huart1; // GPS UART
UART_HandleTypeDef huart2; // Debug/PC Communication UART

GPS_Data gps_data;
IMU_Data imu_data;
Navigation_State nav_state;

uint8_t rx_buffer[256];
uint8_t tx_buffer[256];

/* Function prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
void process_pc_command(uint8_t* command);
void send_gps_data_to_pc(void);
void Error_Handler(void);

/**
 * @brief Main application entry point
 */
int main(void)
{
    /* Initialize HAL Library */
    HAL_Init();
    
    /* Configure system clock */
    SystemClock_Config();
    
    /* Initialize peripherals */
    MX_GPIO_Init();
    MX_USART1_UART_Init(); // GPS module
    MX_USART2_UART_Init(); // PC communication
    
    /* Initialize modules */
    GPS_Init(&huart1);
    IMU_Init();
    Navigation_Init();
    
    /* Send startup message */
    HAL_UART_Transmit(&huart2, (uint8_t*)"STM32 Navigation System Started\r\n", 33, HAL_MAX_DELAY);
    
    /* Main application loop */
    while(1)
    {
        /* Read GPS data */
        if(GPS_Read(&gps_data) == GPS_OK)
        {
            /* Update navigation state */
            Navigation_UpdatePosition(&gps_data);
            
            /* Send data to PC if requested */
            send_gps_data_to_pc();
        }
        
        /* Read IMU data */
        if(IMU_Read(&imu_data) == IMU_OK)
        {
            /* Update navigation with heading information */
            Navigation_UpdateHeading(imu_data.heading);
        }
        
        /* Check for PC commands */
        if(HAL_UART_Receive(&huart2, rx_buffer, sizeof(rx_buffer), 10) == HAL_OK)
        {
            process_pc_command(rx_buffer);
        }
        
        /* Navigation calculations */
        Navigation_Calculate(&nav_state);
        
        /* LED status indication */
        if(gps_data.fix_valid)
        {
            HAL_GPIO_WritePin(LED_GPS_GPIO_Port, LED_GPS_Pin, GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_TogglePin(LED_GPS_GPIO_Port, LED_GPS_Pin);
        }
        
        /* Small delay to prevent excessive CPU usage */
        HAL_Delay(100);
    }
}

/**
 * @brief Process commands received from PC
 */
void process_pc_command(uint8_t* command)
{
    if(strncmp((char*)command, "GPS_REQ", 7) == 0)
    {
        send_gps_data_to_pc();
    }
    else if(strncmp((char*)command, "NAV_START", 9) == 0)
    {
        Navigation_Start();
        HAL_UART_Transmit(&huart2, (uint8_t*)"Navigation started\r\n", 20, HAL_MAX_DELAY);
    }
    else if(strncmp((char*)command, "NAV_STOP", 8) == 0)
    {
        Navigation_Stop();
        HAL_UART_Transmit(&huart2, (uint8_t*)"Navigation stopped\r\n", 20, HAL_MAX_DELAY);
    }
}

/**
 * @brief Send GPS data to PC in NMEA format
 */
void send_gps_data_to_pc(void)
{
    if(!gps_data.fix_valid) return;
    
    /* Format GPGGA sentence */
    int len = snprintf((char*)tx_buffer, sizeof(tx_buffer),
        "$GPGGA,%.2f,%.6f,%c,%.6f,%c,%d,%d,%.1f,%.1f,M,0.0,M,,*00\r\n",
        gps_data.utc_time,
        fabs(gps_data.latitude), (gps_data.latitude >= 0) ? 'N' : 'S',
        fabs(gps_data.longitude), (gps_data.longitude >= 0) ? 'E' : 'W',
        gps_data.fix_valid ? 1 : 0,
        gps_data.satellites_used,
        gps_data.hdop,
        gps_data.altitude
    );
    
    HAL_UART_Transmit(&huart2, tx_buffer, len, HAL_MAX_DELAY);
    
    /* Format GPRMC sentence */
    len = snprintf((char*)tx_buffer, sizeof(tx_buffer),
        "$GPRMC,%.2f,A,%.6f,%c,%.6f,%c,%.2f,%.1f,%.0f,,,A*00\r\n",
        gps_data.utc_time,
        fabs(gps_data.latitude), (gps_data.latitude >= 0) ? 'N' : 'S',
        fabs(gps_data.longitude), (gps_data.longitude >= 0) ? 'E' : 'W',
        gps_data.speed_knots,
        gps_data.course,
        gps_data.date
    );
    
    HAL_UART_Transmit(&huart2, tx_buffer, len, HAL_MAX_DELAY);
}

/**
 * @brief System Clock Configuration
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    
    /* Configure main regulator output voltage */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    
    /* Initialize RCC Oscillators */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 72;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 3;
    
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    
    /* Initialize CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief GPIO Initialization
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Enable GPIO Clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    /* Configure LED pins */
    HAL_GPIO_WritePin(LED_GPS_GPIO_Port, LED_GPS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_RESET);
    
    GPIO_InitStruct.Pin = LED_GPS_Pin|LED_STATUS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
 * @brief USART1 Initialization (GPS Module)
 */
static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief USART2 Initialization (PC Communication)
 */
static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief Error Handler
 */
void Error_Handler(void)
{
    /* Turn on error LED */
    HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET);
    
    /* Infinite loop */
    while(1)
    {
        HAL_GPIO_TogglePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin);
        HAL_Delay(100);
    }
}