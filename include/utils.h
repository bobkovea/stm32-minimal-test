#ifndef UTILS_MY_H
#define UTILS_MY_H

//~1 us @ HSE 8 MHz
static inline __attribute__((always_inline)) void delay_us(uint32_t us)
{
	while(us--)
	{
		__asm("NOP");
	};
}

static inline __attribute__((always_inline)) void delay_ms(const uint32_t ms)
{
	delay_us(ms * 1000U);
}

#endif // UTILS_MY_H