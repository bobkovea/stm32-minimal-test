#ifndef BOARDSETUPUTILS_H
#define BOARDSETUPUTILS_H

#include "stm32f303xc.h"

void ClockInit(void)
{	
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | // GPIOA
				  RCC_AHBENR_GPIOEEN | 	// GPIOE
				  RCC_AHBENR_GPIOCEN | 	// GPIOС
				  RCC_AHBENR_DMA1EN; 	// DMA1
				  
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN |   // USART1
					RCC_APB2ENR_SPI1EN; 	 // SPI1
       
}

// Инициализация GPIO для PE15 (источник: RM0316, разделы 8.4.1, 8.4.7)
void GpioInit(void)
{ 
	/*PORT A*/
	
	 // Настройка пинов SPI1: PA5-SCK, PA6-MISO, PA7-MOSI (уже подключены к L3GD20)
    GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
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
					 
					 
	/* PORT E */
	
	// Настройка пина CS (PE3) как выхода - это КЛЮЧЕВОЙ момент!
    // На Discovery L3GD20 подключен к PE3!
    GPIOE->MODER &= ~GPIO_MODER_MODER3;
    GPIOE->MODER |= (1 << GPIO_MODER_MODER3_Pos);  // Output mode
	
	GPIOE->BSRR |= GPIO_BSRR_BS_3;  // PE3 high
	
	// PE15 - LED ouptut
    GPIOE->MODER &= ~GPIO_MODER_MODER15_Msk;
    GPIOE->MODER |= GPIO_MODER_MODER15_0;
	
	/* PORT C*/
	
    // Настройка PC4 (TX) и PC5 (RX)
    GPIOC->MODER &= ~(GPIO_MODER_MODER4_Msk | GPIO_MODER_MODER5_Msk);
    GPIOC->MODER |= (2 << GPIO_MODER_MODER4_Pos) | (2 << GPIO_MODER_MODER5_Pos);
    
    // Альтернативная функция AF7 для USART1
    GPIOC->AFR[0] &= ~(GPIO_AFRL_AFRL4_Msk | GPIO_AFRL_AFRL5_Msk);
    GPIOC->AFR[0] |= (7 << GPIO_AFRL_AFRL4_Pos) | (7 << GPIO_AFRL_AFRL5_Pos);
 
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

#endif // BOARDSETUPUTILS_H