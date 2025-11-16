#ifndef UTILS_MY_H
#define UTILS_MY_H

// Простая задержка (примерная)
void delay(uint32_t iterations) {
    for(uint32_t i = 0; i < iterations; i++) {
        __asm__("nop");
    }
}

#endif // UTILS_MY_H