
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
#include "dwt_stm32_delay.h"  //Mikrosaniye cinsinden gecikme yapmamizi saglayan kütüphane dahil edildi.


/* Private variables ---------------------------------------------------------*/
uint32_t time; // Zaman degiskeni olusturuldu
uint32_t distance; // Mesafe degiskeni olusturuldu


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);


/* Private user code ---------------------------------------------------------*/
uint32_t Read_HCSR04()
{

		uint32_t local_time=0;
		while(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)); //A portu 7.pin Echo pinine veri gelmedigi sürece Trig pini tetiklenecek
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); //A portu 6.pindeki Trig pini aktif ediliyor
		DWT_Delay_us(10);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); //Trig pini 10 mikrosaniye tetiklendi

		while(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)); //Echo pinine veri geeldiği durumda bu döngüye girilecek.
		{

			local_time++;
			DWT_Delay_us(1); //Ilk degeri 0 olan local_time 1 mikrosaniyede 1 artırıldı.

		}

		return local_time;

}

/**
  * @brief  The application entry point.
  * @retval int
  */





int main(void)
{

  HAL_Init();  // Tüm çevre birimleri sıfırlanır, flash arabirimi ve SysTick timer başlatılır.

  SystemClock_Config(); // Saat konfigürasyonu yapılır

  MX_GPIO_Init(); // Yapılandırılmış çevre birimler başlatılır.
  DWT_Delay_Init();

  while (1)
  {

			time = Read_HCSR04();
			distance = time/58 ; //Kullanilan mesafe sensoru HCSR04 datasheetinde süreyi cm cinsinden degere dönüstürmek için bölünmesi gereken sayi 58 olarak belirtilir.

			if(distance < 450)
			{

				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); // 12.pine bagli kirmizi ledden akım geçecek.
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Kullanilan L298N In4 pini LOW(RESET), In3 pini HIGH(SET) oldugu için saat yönü tersinde dönmeye baslayacak
			}

			if(distance > 450 && distance < 500)
			{

				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); //11.pine bagli sarı ışıktan akım gececek
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); //Kullanılan L298N sürücüsünün In4 pini LOW(RESET), In3 pini de LOW(SET) oldugu için dönmeyecek

			}

			if(distance > 500)
			{

				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); // 15. pine bağlı yeşil ışıktan akım gececek
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); //Kullanılan L298N sürücüsünün In4 pini HIGH(SET), In3 pini LOW(RESET) oldugu için saat yönünde dönmeye başlayacak

			}

	}

}





/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // RCC osilatörler başlatılır
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  // CPU, AHB ve APB veriyolu saatlerin başlatılır
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // GPIO Port saatleri aktif edilir
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();


  //Output olarak girilen A4 A5 A6 A11 A12 A15 pinlerinin konfigürasyonu
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Input olarak girilen A7 pininin konfigürasyonu*/
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {
  }

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

}
#endif
