/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "DIALOG.h"
#include "arm_math.h"
#include "stdio.h"
#include <stdio.h>
#include <math.h>
#include "stm32746g_discovery_audio.h"
#include "arm_const_structs.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFSIZE 2048
#define music_length 48
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

CRC_HandleTypeDef hcrc;

I2C_HandleTypeDef hi2c3;

SAI_HandleTypeDef hsai_BlockA2;
SAI_HandleTypeDef hsai_BlockB2;
DMA_HandleTypeDef hdma_sai2_a;
DMA_HandleTypeDef hdma_sai2_b;

SD_HandleTypeDef hsd1;
DMA_HandleTypeDef hdma_sdmmc1_rx;
DMA_HandleTypeDef hdma_sdmmc1_tx;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
extern void GRAPHICS_HW_Init(void);
extern void GRAPHICS_Init(void);
extern void GRAPHICS_MainTask(void);
static void MX_I2C3_Init(void);
static void MX_DMA_Init(void);
static void MX_SAI2_Init(void);
static void MX_SDMMC1_SD_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */
//uint32_t RBGcolor[2][2][2]={{
//                             {0x0000,0x001F},
//                             {0x07E0,0x07FF},
//                            },
//                            {
//                             {0xF800,0xF81F},
//                             {0xFFE0,0xFFFF},
//                            },
//                           };
uint32_t RBGcolor[2][2][2]={{
                             {0x000000,0x0000FF},
                             {0x00FF00,0x00FFFF},
                            },
                            {
                             {0xFF0000,0xFF00FF},
                             {0xFFFF00,0xFFFFFF},
                            },
                           };
uint16_t sound[8]={1,1912,1704,1517,1433,1276,1136,1012};
struct status state;
uint8_t music[music_length]={0},music_index=0;
//static int16_t music_output[BUFSIZE*2]; //Ë«ÉùµÀÊý¾Ý
static int16_t audio_output_start[BUFSIZE*2]={0}; //¿ªÊ¼Ë«ÉùµÀÊý¾Ý
static int16_t audio_output_0[BUFSIZE*2]={0}; //0Ë«ÉùµÀÊý¾Ý
static int16_t audio_output_none[BUFSIZE*2]={0}; //ÐÅµÀ¹À¼ÆË«ÉùµÀÊý¾Ý
static int16_t audio_output_1[BUFSIZE*2]={0}; //1Ë«ÉùµÀÊý¾Ý
float frequency[4]={18.75,19.21875,19.6875,20.15625};//¿ªÊ¼£¬0£¬ÐÅµÀ¹À¼Æ£¬1
int testdatabase[17] = {0};
bool flag=0,send_flag=0,music_flag=0,play_music_flag=0,send_data_flag=0;
int after_diff[17]={0};
int volumn[2]={0,0};
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void checkTouch(void)
{
    uint8_t buf[6];
    HAL_I2C_Mem_Read(&hi2c3, 0x70, 0x02, 1, buf, 1, 1000);
    if(buf[0]>0 && buf[0]<6)
    {
        HAL_I2C_Mem_Read(&hi2c3, 0x70, 0x03, 1, buf, 4, 1000);
        GUI_TOUCH_StoreState((buf[2]<<8|buf[3])&0x0fff, (buf[0]<<8|buf[1])&0x0fff);
    }
    else
    {
        GUI_TOUCH_StoreState(-1,-1);
    }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
  //checkTouch();
  __HAL_TIM_SetAutoreload(&htim3,sound[music[music_index]]);
  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,sound[music[music_index]]/2);
  for(int count1=0;count1<1000;count1++)
      for(int count2=0;count2<100;count2++);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  music_index=(music_index+1)%music_length;
}
void diff_code(int data[17])
{
  after_diff[0]=data[0];
  for(int i=1;i<17;i++)
  {
    after_diff[i]=data[i];//(data[i]==data[i-1]);
  }
}
/*void calnumber(int output[17])
{
  volumn[0]=2;
  volumn[1]=1;
  for(int i=0;i<8;i++)
  {
    volumn[0]+=output[i];
  }
  for(int i=8;i<17;i++)
  {
    volumn[1]+=output[i];
  }
}
void clean_elements(void)
{
  for(int j=0;j<BUFSIZE*2;j++)
  {
    audio_output[j]=0;
    audio_output_2nd[j]=0;
  }
}
*/
void stop_send(void)
{
  BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
}
void send(int data[17])
{
  int i;
  BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
  BSP_AUDIO_OUT_Play((uint16_t*)audio_output_start, BUFSIZE*2);
  GUI_Delay(30);
  BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
  for(i=0;i<17;i++)
  {
    if(data[i])
    {
      BSP_AUDIO_OUT_Play((uint16_t*)audio_output_1, BUFSIZE*2);
      GUI_Delay(30);
      BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    }
    else
    {
      BSP_AUDIO_OUT_Play((uint16_t*)audio_output_0, BUFSIZE*2);
      GUI_Delay(30);
      BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    }
  }
  //BSP_AUDIO_OUT_Play((uint16_t*)audio_output_none, BUFSIZE*2);
  //HAL_Delay(650);
} 
/*void send(int data[17])
{
  BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
  calnumber(data);
  clean_elements();
  for(int j=0;j<8;j++)
  {
    if(data[j])
    {
      for(int i=0;i<BUFSIZE;i++)
      {
        audio_output[i*2] += (16384/volumn[0])* sin(2*3.1415926535 *(frequency[j+1])/ 96 * i); //×óÉùµÀ 48KSPS,5kHz ÕýÏÒ²¨
        audio_output[i*2+1] +=  (16384/volumn[0])* sin(2*3.1415926535 *(frequency[j+1])/ 96 * i); //ÓÒÉùµÀ 48KSPS,20kHz ÕýÏÒ²¨
      } 
    }
  }
  for(int i=0;i<BUFSIZE;i++)
  {
    audio_output[i*2] += (16384/volumn[0]) * sin(2*3.1415926535 *(frequency[0])/ 96 * i); //×óÉùµÀ 48KSPS,5kHz ÕýÏÒ²¨
    audio_output[i*2+1] += (16384/volumn[0]) * sin(2*3.1415926535 *(frequency[0])/ 96 * i); //ÓÒÉùµÀ 48KSPS,20kHz ÕýÏÒ²¨
    audio_output[i*2] += (16384/volumn[0]) * sin(2*3.1415926535 *(13.125)/ 96 * i); //×óÉùµÀ 48KSPS,5kHz ÕýÏÒ²¨
    audio_output[i*2+1] += (16384/volumn[0]) * sin(2*3.1415926535 *(13.125)/ 96 * i); //ÓÒÉùµÀ 48KSPS,20kHz ÕýÏÒ²¨
    audio_output_2nd[i*2] += (16384/volumn[1]) * sin(2*3.1415926535 *(13.125)/ 96 * i); //×óÉùµÀ 48KSPS,5kHz ÕýÏÒ²¨
    audio_output_2nd[i*2+1] += (16384/volumn[1]) * sin(2*3.1415926535 *(13.125)/ 96 * i); //ÓÒÉùµÀ 48KSPS,20kHz ÕýÏÒ²¨
  }
  for(int j=0;j<9;j++)
  {
    if(data[j+8])
    {
      for(int i=0;i<BUFSIZE;i++)
      {
        audio_output_2nd[i*2] += (16384/volumn[1])* sin(2*3.1415926535 *(frequency[j+1])/ 96 * i); //×óÉùµÀ 48KSPS,5kHz ÕýÏÒ²¨
        audio_output_2nd[i*2+1] +=  (16384/volumn[1])* sin(2*3.1415926535 *(frequency[j+1])/ 96 * i); //ÓÒÉùµÀ 48KSPS,20kHz ÕýÏÒ²¨
      } 
    }
  }
  send_data_flag=0;
  BSP_AUDIO_OUT_Play((uint16_t*)audio_output, BUFSIZE*2);
}
void change_send_data(void)
{
  if(send_data_flag)
  {
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    //GUI_Delay(1000);
    BSP_AUDIO_OUT_Play((uint16_t*)audio_output, BUFSIZE*2);
  }
  else
  {
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    //GUI_Delay(1000);
    BSP_AUDIO_OUT_Play((uint16_t*)audio_output_2nd, BUFSIZE*2);
  }
  send_data_flag=!send_data_flag;
}
bool clean_elements(void)
{
  for(int j=0;j<BUFSIZE*2;j++)
  {
    if(flag) {audio_output[j]=0; flag=0;}
    else {audio_output_2nd[j]=0; flag=1;}
  }
  return flag;
}

//void play_music(void)
//{
//    BSP_AUDIO_OUT_Play((uint16_t*)music_output, BUFSIZE*2);
////  flag=!flag;
////  switch((int)flag)
////  {
////  case 0:
////    for(int i=0;i<BUFSIZE;i++)
////    {
////      audio_output[i*2] = 0;//2*964 * sin(2*3.1415926535 *(13.125)/ 96 * i); //×óÉùµÀ 48KSPS,5kHz ÕýÏÒ²¨
//////      audio_output[i*2+1] = 2*964 * sin(2*3.1415926535 *(13.125)/ 96 * i); //ÓÒÉùµÀ 48KSPS,20kHz ÕýÏÒ²¨
////    }
////    BSP_AUDIO_OUT_Play((uint16_t*)audio_output, BUFSIZE*2);
////    //HAL_Delay(300);
////    break;
////  case 1:
////    for(int i=0;i<BUFSIZE;i++)
////    {
////      audio_output_2nd[i*2] = music[i];//2*964 * sin(2*3.1415926535 *(13.125)/ 96 * i); //×óÉùµÀ 48KSPS,5kHz ÕýÏÒ²¨
//////      audio_output_2nd[i*2+1] += 2*964 * sin(2*3.1415926535 *(13.125)/ 96 * i); //ÓÒÉùµÀ 48KSPS,20kHz ÕýÏÒ²¨
////    }
////    BSP_AUDIO_OUT_Play((uint16_t*)audio_output_2nd, BUFSIZE*2);
////    //HAL_Delay(300);
////    break;
////  }
//}
void send(int data[17])
{
  switch((int)clean_elements())
  {
  case 0:
    for(int j=0;j<17;j++)
    {
      if(data[j])
      {
        for(int i=0;i<BUFSIZE;i++)
        {
          //audio_output[i*2] += 0;//2*964 * sin(2*3.1415926535 *(frequency[j])/ 96 * i); //×óÉùµÀ 48KSPS,5kHz ÕýÏÒ²¨
          audio_output[i*2+1] += 2*964 * sin(2*3.1415926535 *(frequency[j])/ 96 * i); //ÓÒÉùµÀ 48KSPS,20kHz ÕýÏÒ²¨
        } 
      }
    }
    for(int i=0;i<BUFSIZE;i++)
    {
      //audio_output[i*2] = music[i];//2*964 * sin(2*3.1415926535 *(13.125)/ 96 * i); //×óÉùµÀ 48KSPS,5kHz ÕýÏÒ²¨
      audio_output[i*2+1] += 2*964 * sin(2*3.1415926535 *(13.125)/ 96 * i); //ÓÒÉùµÀ 48KSPS,20kHz ÕýÏÒ²¨
    }
    BSP_AUDIO_OUT_Play((uint16_t*)audio_output, BUFSIZE*2);
    //HAL_Delay(300);
    break;
  case 1:
    for(int j=0;j<17;j++)
    {
      if(data[j])
      {
        for(int i=0;i<BUFSIZE;i++)
        {
          //audio_output_2nd[i*2] += 0;//2*964 * sin(2*3.1415926535 *(frequency[j])/ 96 * i); //×óÉùµÀ 48KSPS,5kHz ÕýÏÒ²¨
          audio_output_2nd[i*2+1] += 2*964 * sin(2*3.1415926535 *(frequency[j])/ 96 * i); //ÓÒÉùµÀ 48KSPS,20kHz ÕýÏÒ²¨
        } 
      }
    }
    for(int i=0;i<BUFSIZE;i++)
    {
      //audio_output_2nd[i*2] = music[i];//2*964 * sin(2*3.1415926535 *(13.125)/ 96 * i); //×óÉùµÀ 48KSPS,5kHz ÕýÏÒ²¨
      audio_output_2nd[i*2+1] += 2*964 * sin(2*3.1415926535 *(13.125)/ 96 * i); //ÓÒÉùµÀ 48KSPS,20kHz ÕýÏÒ²¨
    }
    BSP_AUDIO_OUT_Play((uint16_t*)audio_output_2nd, BUFSIZE*2);
    //HAL_Delay(300);
    break;
  }
}
*/
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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_I2C3_Init();
  MX_DMA_Init();
  MX_SAI2_Init();
  MX_SDMMC1_SD_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  retSD = f_mount (&SDFatFS, "", 0);
  //printf("%d\n",retSD);
  if(retSD==FR_OK)
  {
    //printf("mount OK\n");
    retSD = f_open (&SDFile, "music.txt", FA_OPEN_ALWAYS|FA_READ);
    //printf("%d\n",retSD);
    if(retSD==FR_OK)
    {
      //printf("open OK\n");
      retSD = f_read (&SDFile, music, music_length, 0);
      //printf("read ret:%d\n", retSD);
    }
    retSD = f_close (&SDFile);
    //printf("close ret:%d\n", retSD);
  }
  for(int i=0;i<music_length;i++)
  {
    music[i]=music[i]-'0';
  }
  HAL_TIM_Base_Start_IT(&htim4);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  
  for(int i=0;i<BUFSIZE;i++)
  {
    audio_output_start[i*2] += (16384)* sin(2*3.1415926535 *(frequency[0])/ 96 * i); //×óÉùµÀ 48KSPS
    audio_output_start[i*2+1] += (16384)* sin(2*3.1415926535 *(frequency[0])/ 96 * i); //ÓÒÉùµÀ 48KSPS
    audio_output_0[i*2] += (16384)* sin(2*3.1415926535 *(frequency[1])/ 96 * i); //×óÉùµÀ 48KSPS
    audio_output_0[i*2+1] += (16384)* sin(2*3.1415926535 *(frequency[1])/ 96 * i); //ÓÒÉùµÀ 48KSPS
    audio_output_1[i*2] += (16384)* sin(2*3.1415926535 *(frequency[3])/ 96 * i); //×óÉùµÀ 48KSPS
    audio_output_1[i*2+1] += (16384)* sin(2*3.1415926535 *(frequency[3])/ 96 * i); //ÓÒÉùµÀ 48KSPS
  }
//  for(int i=0;i<BUFSIZE;i++)
//  {
//    audio_output_start[i*2] += (16384/2)* sin(2*3.1415926535 *(frequency[2])/ 96 * i); //×óÉùµÀ 48KSPS
//    audio_output_start[i*2+1] += (16384/2)* sin(2*3.1415926535 *(frequency[2])/ 96 * i); //ÓÒÉùµÀ 48KSPS
//    audio_output_0[i*2] += (16384/2)* sin(2*3.1415926535 *(frequency[2])/ 96 * i); //×óÉùµÀ 48KSPS
//    audio_output_0[i*2+1] += (16384/2)* sin(2*3.1415926535 *(frequency[2])/ 96 * i); //ÓÒÉùµÀ 48KSPS
//    audio_output_none[i*2] += (16384/2)* sin(2*3.1415926535 *(frequency[2])/ 96 * i); //×óÉùµÀ 48KSPS
//    audio_output_none[i*2+1] += (16384/2)* sin(2*3.1415926535 *(frequency[2])/ 96 * i); //ÓÒÉùµÀ 48KSPS
//    audio_output_1[i*2] += (16384/2)* sin(2*3.1415926535 *(frequency[2])/ 96 * i); //×óÉùµÀ 48KSPS
//    audio_output_1[i*2+1] += (16384/2)* sin(2*3.1415926535 *(frequency[2])/ 96 * i); //ÓÒÉùµÀ 48KSPS
//  }
  BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, 100, SAI_AUDIO_FREQUENCY_96K);
  BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_13);
  //BSP_AUDIO_OUT_Play((uint16_t*)audio_output_none, BUFSIZE*2);
  diff_code(testdatabase);
  send(after_diff); 
  //play_music();
  send_flag=1;
  /* USER CODE END 2 */

/* Initialise the graphical hardware */
  GRAPHICS_HW_Init();

  /* Initialise the graphical stack engine */
  GRAPHICS_Init();
  
  /* Graphic application */  
  GRAPHICS_MainTask();
    
  /* Infinite loop */
  for(;;);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_SAI2|RCC_PERIPHCLK_I2C3
                              |RCC_PERIPHCLK_SDMMC1|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 344;
  PeriphClkInitStruct.PLLI2S.PLLI2SP = RCC_PLLP_DIV2;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  PeriphClkInitStruct.PLLI2S.PLLI2SQ = 7;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 200;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
  PeriphClkInitStruct.PLLI2SDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
  PeriphClkInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInitStruct.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x6000030D;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief SAI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SAI2_Init(void)
{

  /* USER CODE BEGIN SAI2_Init 0 */

  /* USER CODE END SAI2_Init 0 */

  /* USER CODE BEGIN SAI2_Init 1 */

  /* USER CODE END SAI2_Init 1 */
  hsai_BlockA2.Instance = SAI2_Block_A;
  hsai_BlockA2.Init.Protocol = SAI_FREE_PROTOCOL;
  hsai_BlockA2.Init.AudioMode = SAI_MODEMASTER_TX;
  hsai_BlockA2.Init.DataSize = SAI_DATASIZE_16;
  hsai_BlockA2.Init.FirstBit = SAI_FIRSTBIT_MSB;
  hsai_BlockA2.Init.ClockStrobing = SAI_CLOCKSTROBING_RISINGEDGE;
  hsai_BlockA2.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA2.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLE;
  hsai_BlockA2.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA2.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;
  hsai_BlockA2.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
  hsai_BlockA2.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockA2.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA2.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA2.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  hsai_BlockA2.FrameInit.FrameLength = 64;
  hsai_BlockA2.FrameInit.ActiveFrameLength = 32;
  hsai_BlockA2.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
  hsai_BlockA2.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  hsai_BlockA2.FrameInit.FSOffset = SAI_FS_FIRSTBIT;
  hsai_BlockA2.SlotInit.FirstBitOffset = 0;
  hsai_BlockA2.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
  hsai_BlockA2.SlotInit.SlotNumber = 4;
  hsai_BlockA2.SlotInit.SlotActive = 0x0000FFFF;
  if (HAL_SAI_Init(&hsai_BlockA2) != HAL_OK)
  {
    Error_Handler();
  }
  hsai_BlockB2.Instance = SAI2_Block_B;
  hsai_BlockB2.Init.Protocol = SAI_FREE_PROTOCOL;
  hsai_BlockB2.Init.AudioMode = SAI_MODESLAVE_RX;
  hsai_BlockB2.Init.DataSize = SAI_DATASIZE_16;
  hsai_BlockB2.Init.FirstBit = SAI_FIRSTBIT_MSB;
  hsai_BlockB2.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
  hsai_BlockB2.Init.Synchro = SAI_SYNCHRONOUS;
  hsai_BlockB2.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockB2.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;
  hsai_BlockB2.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockB2.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB2.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB2.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  hsai_BlockB2.FrameInit.FrameLength = 64;
  hsai_BlockB2.FrameInit.ActiveFrameLength = 32;
  hsai_BlockB2.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
  hsai_BlockB2.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  hsai_BlockB2.FrameInit.FSOffset = SAI_FS_FIRSTBIT;
  hsai_BlockB2.SlotInit.FirstBitOffset = 0;
  hsai_BlockB2.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
  hsai_BlockB2.SlotInit.SlotNumber = 4;
  hsai_BlockB2.SlotInit.SlotActive = 0x0000FFFF;
  if (HAL_SAI_Init(&hsai_BlockB2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SAI2_Init 2 */

  /* USER CODE END SAI2_Init 2 */

}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_1B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 108-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 108-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_DOWN;
  htim3.Init.Period = 2000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 10800-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 2500-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  /* DMA2_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOK, GPIO_PIN_3, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_12, GPIO_PIN_SET);

  /*Configure GPIO pin : PG13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PK3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /*Configure GPIO pins : PI1 PI12 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
