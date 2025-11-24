#ifndef BOARDSETUPUTILS_H
#define BOARDSETUPUTILS_H

#include "stm32f303xc.h"

void SwitchToExternalClock()
{
	RCC->CR |= RCC_CR_HSEBYP | RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSE;
	
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE);
}

void SetPeripheralClock(const uint8_t enable)
{
	const uint32_t ahb_enr = RCC_AHBENR_GPIOAEN |     // GPIOA
							RCC_AHBENR_GPIOBEN |    // GPIOB
							RCC_AHBENR_GPIOCEN | 	// GPIOС
							RCC_AHBENR_GPIOEEN | 	// GPIOE
							RCC_AHBENR_DMA1EN; 	    // DMA1
	
	const uint32_t apb1_enr = RCC_APB1ENR_I2C1EN | // I2C1
							RCC_APB1ENR_PWREN; // PWR
							
    const uint32_t apb2_enr = RCC_APB2ENR_USART1EN |   // USART1
							RCC_APB2ENR_SPI1EN | 	 // SPI1
							RCC_APB2ENR_SYSCFGEN; // SYSCFG
							
	if(enable)
	{
		RCC->AHBENR |= ahb_enr;
		RCC->APB1ENR |= apb1_enr;
		RCC->APB2ENR |= apb2_enr;

	}
	else
	{
		RCC->AHBENR &= ~ahb_enr;
		RCC->APB1ENR &= ~apb1_enr;
		RCC->APB2ENR &= ~apb2_enr;
	}
}

void ClockInit(void)
{	
	SwitchToExternalClock();
	SetPeripheralClock(1);
}
	

void GpioInit(void)
{ 
	/* PORT A */
	
	 // Настройка пинов SPI1: PA5-SCK, PA6-MISO, PA7-MOSI (уже подключены к L3GD20)
    GPIOA->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    GPIOA->MODER |= (2 << GPIO_MODER_MODER5_Pos) |  // Alternate function
                    (2 << GPIO_MODER_MODER6_Pos) |
                    (2 << GPIO_MODER_MODER7_Pos);
    
    // Альтернативная функция 5 для SPI1
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL5 | GPIO_AFRL_AFRL6 | GPIO_AFRL_AFRL7);
    GPIOA->AFR[0] |= (5 << GPIO_AFRL_AFRL5_Pos) |
                     (5 << GPIO_AFRL_AFRL6_Pos) |
                     (5 << GPIO_AFRL_AFRL7_Pos);
					 
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_5 | GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7); // Push-pull
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR5 | GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR7); // No pull-up/pull-down 
	
	/* PORT B */
	
	 // Настройка PB6 (SCL) и PB7 (SDA) как альтернативная функция с открытым стоком
    // Настройка PB6 - I2C1_SCL, PB7 - I2C1_SDA
    GPIOB->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    GPIOB->MODER |= (2 << GPIO_MODER_MODER6_Pos) | (2 << GPIO_MODER_MODER7_Pos);
    
    GPIOB->OTYPER |= GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7;  // Open-drain
    GPIOB->AFR[0] |= (4 << GPIO_AFRL_AFRL6_Pos) | (4 << GPIO_AFRL_AFRL7_Pos);  // AF4

	GPIOB->PUPDR |= (1 << GPIO_PUPDR_PUPDR6_Pos) | (1 << GPIO_PUPDR_PUPDR7_Pos);
	
	/* PORT C */
	
    // Настройка PC4 (TX) и PC5 (RX)
    GPIOC->MODER &= ~(GPIO_MODER_MODER4_Msk | GPIO_MODER_MODER5_Msk);
    GPIOC->MODER |= (2 << GPIO_MODER_MODER4_Pos) | (2 << GPIO_MODER_MODER5_Pos);
    
    // Альтернативная функция AF7 для USART1
    GPIOC->AFR[0] &= ~(GPIO_AFRL_AFRL4_Msk | GPIO_AFRL_AFRL5_Msk);
    GPIOC->AFR[0] |= (7 << GPIO_AFRL_AFRL4_Pos) | (7 << GPIO_AFRL_AFRL5_Pos);
	
	/* PORT E */
	
	// Настройка пина CS (PE3) как выхода
    // На Discovery L3GD20 подключен к PE3!
    GPIOE->MODER &= ~GPIO_MODER_MODER3;
    GPIOE->MODER |= (1 << GPIO_MODER_MODER3_Pos);  // Output mode
	
	GPIOE->BSRR |= GPIO_BSRR_BS_3;  // PE3 high
	
	// PE15 - LED output
    GPIOE->MODER &= ~GPIO_MODER_MODER15_Msk;
    GPIOE->MODER |= GPIO_MODER_MODER15_0;
}

void UsartInit(void)
{
    // Настройка UART: 115200 8N1
    USART1->BRR = 8000000 / 115200;
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
    
    // Включение DMA для передачи в USART1
    USART1->CR3 |= USART_CR3_DMAT;
    
    // Настройка DMA1 Channel4 (для USART1_TX)
    DMA1_Channel4->CCR &= ~DMA_CCR_EN;  // Выключить перед настройкой
	 
    DMA1_Channel4->CPAR = (uint32_t)&(USART1->TDR);  // Периферийный адрес
    DMA1_Channel4->CCR = DMA_CCR_MINC |              // Инкремент адреса памяти
                         DMA_CCR_DIR; 				 // Направление: память->периферия            
}

void SpiInit(void)
{
	SPI1->CR1 &= ~SPI_CR1_SPE; // Disable SPI
	
	// CPOL=0, CPHA=0 (Mode 0)
    // MSB first
    SPI1->CR1 = SPI_CR1_MSTR |  // Master mode
                SPI_CR1_BR_1 |  // Baudrate = f_clk/8 (1 MHz @ f_clk = 8 MHz)
                SPI_CR1_SSM | SPI_CR1_SSI; // Software NSS management 
	
    SPI1->CR2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0 | // 8-bit data size
				SPI_CR2_FRXTH; // FIFO reception threshold (triggers RXNE event)

	SPI1->CR1 |= SPI_CR1_SPE; // Enable SPI
}

void I2cInit(void) {
	 // Отключение I2C перед настройкой
    I2C1->CR1 &= ~I2C_CR1_PE;
	
    // Настройка таймингов для 100kHz
    I2C1->TIMINGR = (1 << I2C_TIMINGR_PRESC_Pos)   |
                    (3 << I2C_TIMINGR_SCLDEL_Pos) |
                    (1 << I2C_TIMINGR_SDADEL_Pos) |
                    (3 << I2C_TIMINGR_SCLH_Pos)   |
                    (9 << I2C_TIMINGR_SCLL_Pos);
					
    // Включение I2C
    I2C1->CR1 |= I2C_CR1_PE;
}

void InterruptsInit(void)
{
	/* EXTI0 PA0 */
	
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0_Msk;
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;
    
    EXTI->RTSR |= EXTI_RTSR_TR0; 
    EXTI->IMR |= EXTI_IMR_MR0;
    
    NVIC_SetPriority(EXTI0_IRQn, 0);
    NVIC_EnableIRQ(EXTI0_IRQn);
	
	/* EXTI20 RTC Wakeup */
	
    EXTI->RTSR |= EXTI_RTSR_TR20;  // Rising edge trigger
	EXTI->IMR |= EXTI_IMR_MR20;    // Разрешить прерывание
    
    NVIC_SetPriority(RTC_WKUP_IRQn, 0);
    NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

void WakeupInit(void)
{
	PWR->CSR |= PWR_CSR_EWUP1;  // Enable Wakeup pin (PA0)
}

void RtcInit(void)
{	
	// Разрешить доступ к Backup domain
	PWR->CR |= PWR_CR_DBP;  
	
	// Включить LSI (внутренний источник на 40 kHz)
    RCC->CSR |= RCC_CSR_LSION;
    while(!(RCC->CSR & RCC_CSR_LSIRDY));
	
	// Настроить RTC на тактирование от LSI
    RCC->BDCR &= ~RCC_BDCR_RTCSEL;
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSI; 
	
	// Включить тактирование RTC
    RCC->BDCR |= RCC_BDCR_RTCEN; 
    
	// Разблокировать настройку RTC
    RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	
	/*
	// Настройки для режима 1 Hz
	RTC->ISR |= RTC_ISR_INIT;
    while (!(RTC->ISR & RTC_ISR_INITF));

    RTC->PRER = (127 << RTC_PRER_PREDIV_A_Pos) | 
                (255 << RTC_PRER_PREDIV_S_Pos);

    RTC->ISR &= ~RTC_ISR_INIT;
	*/
	
	RTC->CR &= ~RTC_CR_WUTE;
    while (!(RTC->ISR & RTC_ISR_WUTWF));
	
	RTC->WUTR = 2500U; // 1Hz
	
    RTC->CR &= ~RTC_CR_WUCKSEL; // RTC_CLK = LSI_FREQ / 16 = 2500 Hz
	
    RTC->CR |= RTC_CR_WUTIE; // Wake-up interrupts enable
	
	RTC->WPR = 0xFF;
}


#endif // BOARDSETUPUTILS_H