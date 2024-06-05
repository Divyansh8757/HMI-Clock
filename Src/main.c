/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdio.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define slvaddr 0x7C
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
/* USER CODE BEGIN PFP */
void mdelay(uint16_t tim);
void lcd_Init();
void lcd_send_cmd(char data);
void lcd_send_data(char data);
void lcd_send_string (char *str);
void lcd_set_cursor(int row, int col);
void lcd_clear();
void set_time(uint8_t hour, uint8_t* minute, char tm, bool temp_set_t, bool temp_set_a);
void display_time(uint8_t hout, uint8_t* minute, char tm);
void enter(uint8_t new_hour, uint8_t* new_minute, char new_tm, bool new_set_t, bool new_set_a);
void sound_alarm();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t hour=0;
uint8_t minute[2]={0,0};
int row = 1;
char tm='A';
char Time[7];
bool set_t = false;
bool set_a = false;
bool alarm = false;
uint8_t a_hour=0;
uint8_t a_minute[2]={0,0};
char a_tm;
int sec=0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the System Power */
  SystemPower_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_I2C1_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start(&htim6);
  HAL_TIM_Base_Start(&htim7);
  lcd_Init();
  set_time(hour,minute,tm,set_t,set_a);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(set_t==true){
		  set_time(hour,minute,tm,set_t,set_a);
	  }
	  else if(set_a==true){
		  set_time(hour,minute,tm,set_t,set_a);
	  }
	  else if((alarm==true)&&(hour==a_hour)&&(minute[0]==a_minute[0])&&(minute[1]==a_minute[1])&&(tm==a_tm)){
		  sound_alarm();
	  }
	  else{
		  if(TIM7->CNT>=999){
			  sec++;
		  }
		  else if(sec>59){
			  sec=0;
			  minute[1]++;
		  }
		  else if(minute[1]>9){
			  minute[1]=0;
			  minute[0]++;
			  lcd_Init();
		  }
		  else if(minute[0]>5){
			  hour++;
			  minute[0]=0;
			  if((hour>11) && (minute[1]==0) && (minute[0]=0)){
				  if(tm=='A')
					  tm='P';
				  else if(tm=='P')
					  tm='A';
			  }
		  }
		  else if(hour>12){
			  hour=1;
		  }

		  display_time(hour,minute,tm);
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_4;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{

  /*
   * Switch to SMPS regulator instead of LDO
   */
  if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK)
  {
    Error_Handler();
  }
/* USER CODE BEGIN PWR */
/* USER CODE END PWR */
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00000004;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 4000-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 4000-1;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 1000;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_BUTTON_Pin */
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PC0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI8_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI8_IRQn);

  HAL_NVIC_SetPriority(EXTI9_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_IRQn);

  HAL_NVIC_SetPriority(EXTI13_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI13_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void mdelay(uint16_t tim){
	__HAL_TIM_SET_COUNTER(&htim6,0);
	while(__HAL_TIM_GET_COUNTER(&htim6)<tim);
}

void lcd_send_cmd(char data){
	uint8_t cmd[2];
	cmd[0] &= 0x00;
	cmd[1] &= 0x00;
	cmd[1] |= data;
	HAL_I2C_Master_Transmit(&hi2c1, slvaddr, cmd, 2, 100);
}

void lcd_send_data(char data){
	uint8_t cmd[2];
	cmd[0] &= 0x00;
	cmd[0] |= 0x40;
	cmd[1] &= 0x00;
	cmd[1] |= data;
	HAL_I2C_Master_Transmit(&hi2c1, slvaddr, cmd, 2, 100);
}

void lcd_clear(){
	lcd_send_cmd(0x01);
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}

void lcd_set_cursor(int row, int col){
	switch(row){
	case 1:
		col |= 0x80;
		break;
	case 2:
		col |= 0xC0;
		break;
	}
	lcd_send_cmd(col);
	col=0;
}

void lcd_Init(){
	lcd_send_cmd(0x2C);
	mdelay(1);
	lcd_send_cmd(0x0C);
	mdelay(1);
	lcd_send_cmd(0x01);
	mdelay(1);
	lcd_send_cmd(0x06);
	mdelay(1);
}

void display_time(uint8_t hour, uint8_t* minute, char tm){
	lcd_set_cursor(1,0);
	if(hour<10){
		sprintf(Time, "0%d:%d%d%c", hour, minute[0],minute[1],tm);
		lcd_send_string(Time);
	}
	else{
		sprintf(Time, "%d:%d%d%c", hour, minute[0],minute[1],tm);
		lcd_send_string(Time);
	}
}

void sound_alarm(){
	while((HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==0) || (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==0)){
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,1);
		lcd_Init();
		HAL_Delay(1);
		lcd_send_string("ALARM!");
		HAL_Delay(1000);
		lcd_clear();
		HAL_Delay(500);
	}
	if((HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==1)){
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);
		a_minute[1]+=5;
		if(a_minute[1]>9){
			a_minute[1]=a_minute[1]-10;
			a_minute[0]++;
		}
		else if(a_minute[0]>5){
			a_minute[0]=0;
			a_hour++;
		}
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==1){
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);
		alarm=false;
		a_hour=0;
		a_minute[0]=0;
		a_minute[1]=0;
	}
}

void set_time(uint8_t hour, uint8_t* minute,char tm, bool temp_set_t, bool temp_set_a){
	lcd_Init();
	mdelay(1);
	display_time(hour,minute,tm);
	uint8_t temp_hour = hour;
	uint8_t temp_minute[2]={'0'};
	char temp_tm=tm;
	temp_minute[0]=minute[0];
	temp_minute[1]=minute[1];
	lcd_send_cmd(0x0f);
	mdelay(1);
	lcd_set_cursor(1,1);
	while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)!=1){
		if(row==1){
			while((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==0) && (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==0) && (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)!=1)){
				// Wait for arrow keys to be pressed
			}
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==1 && (row==1)){
				if(temp_hour==12){
					temp_hour=1;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				else{
					temp_hour++;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				mdelay(200);
			}
			else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==1 && (row==1)){
				if(temp_hour==1){
					temp_hour=12;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				else{
					temp_hour--;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				mdelay(200);
			}
		}
		else if(row==3){
			while((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==0) && (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==0) && (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)!=1)){
				// Wait for arrow keys to be pressed
			}
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==1 && (row==3)){
				if(temp_minute[0]==5){
					temp_minute[0]=0;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				else{
					temp_minute[0]++;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				mdelay(200);
			}
			else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)==1 && (row==3)){
				if(temp_minute[0]==0){
					temp_minute[0]=5;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				else{
					temp_minute[0]--;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				mdelay(200);
			}
		}
			else if(row==4){
				while((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==0) && (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==0) && (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)!=1)){
					// Wait for arrow keys to be pressed
				}
				if((HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==1) && (row==4)){
					if(temp_minute[1]==9){
						temp_minute[1]=0;
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					else{
						temp_minute[1]++;
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					mdelay(200);
				}
				else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==1 && (row==4)){
					if(temp_minute[1]==0){
						temp_minute[1]=9;
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					else{
						temp_minute[1]--;
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					mdelay(200);
				}
			}
			else if(row==5){
				while((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==0) && (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==0) && (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)!=1)){
					// Wait for arrow keys to be pressed
				}
				if((HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==1) && (row==5)){
					if(temp_tm=='A'){
						temp_tm='P';
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					else if(temp_tm=='P'){
						temp_tm = 'A';
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					mdelay(200);
				}
				else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==1 && (row==5)){
					if(temp_tm=='A'){
						temp_tm='P';
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					else if(temp_tm=='P'){
						temp_tm = 'A';
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					mdelay(200);
				}
			}
	}
	enter(temp_hour, temp_minute, temp_tm, temp_set_t, temp_set_a);
}

void enter(uint8_t new_hour, uint8_t* new_minute, char new_tm, bool new_set_t, bool new_set_a){
	lcd_Init();
	row=1;
	mdelay(500);
	if(new_set_t){
		hour=new_hour;
		minute[0]=new_minute[0];
		minute[1]=new_minute[1];
		tm = new_tm;
		display_time(hour,minute,tm);
		set_t = false;
	}
	else if(new_set_a){
		a_hour=new_hour;
		a_minute[0]=new_minute[0];
		a_minute[1]=new_minute[1];
		a_tm=new_tm;
		set_a=false;
		alarm=true;
	}
	else{
		hour=new_hour;
		minute[0]=new_minute[0];
		minute[1]=new_minute[1];
		tm = new_tm;
		display_time(hour,minute,tm);
}
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin==GPIO_PIN_0){
		if(row==1){
			row=5;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		else if (row==3){
			row-=2;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		else if (row==4){
			row-=1;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		else if(row==5){
			row-=1;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		mdelay(100);
	}
	else if((GPIO_Pin==GPIO_PIN_1)){
		if(row==1){
			row+=2;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		else if(row==3){
			row+=1;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		else if(row==4){
			row+=1;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		else if(row==5){
			row=1;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		mdelay(100);
	}
	else if(GPIO_Pin==GPIO_PIN_8){
		set_t = true;
		set_a = false;
	}
	else if(GPIO_Pin==GPIO_PIN_9){
		set_t = false;
		set_a = true;
		alarm = true;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
