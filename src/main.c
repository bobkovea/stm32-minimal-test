#include "stm32f303xc.h"

// Простая задержка (примерная)
void delay(uint32_t iterations) {
    for(uint32_t i = 0; i < iterations; i++) {
        __asm__("nop");
    }
}

// Инициализация GPIO для PE15 (источник: RM0316, разделы 8.4.1, 8.4.7)
void GpioInit(void)
{ 
	/* PORT E */
	
	// PE15 - Output
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

void ClockInit(void)
{	
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN | // GPIOE
				  RCC_AHBENR_GPIOCEN | // GPIOС
				  RCC_AHBENR_DMA1EN; // DMA1
		
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;   // USART1
       
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
// Отправка данных через DMA
void UART1_DMA_Send(uint8_t* data, uint16_t length)
{
	while (DMA1_Channel4->CNDTR != 0 && (DMA1_Channel4->CCR & DMA_CCR_EN));
	
	DMA1_Channel4->CCR &= ~DMA_CCR_EN;
	DMA1_Channel4->CMAR = (uint32_t)data;
    DMA1_Channel4->CNDTR = length;  // Количество данных
    DMA1_Channel4->CCR |= DMA_CCR_EN;  // Запуск DMA
}

// Отправка строки через DMA
void UART1_DMA_SendString(char* str) {
    uint16_t len = 0;
    while (str[len] != '\0') len++;
    UART1_DMA_Send((uint8_t*)str, len);
}


int main(void) {
	
    ClockInit();
    GpioInit();
	UsartInit();
    
	char message1[] = "Hello DMA!\r\n";
    char message2[] = "Second message!\r\n";
	
    // Основной цикл - мигаем PE15
    while(1) {
        // Включаем PE15 (устанавливаем высокий уровень)
        // Используем BSRR для атомарной установки бита
        GPIOE->BSRR = GPIO_BSRR_BS_15;	
		UART1_DMA_SendString(message1);	
        delay(500000);
		
        // Выключаем PE15 (устанавливаем низкий уровень)  
        GPIOE->BSRR = GPIO_BSRR_BR_15;
		UART1_DMA_SendString(message2);
		
        delay(500000);
    }
}