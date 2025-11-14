/* Конфигурация памяти (источник: DS9116, раздел 4) */
MEMORY
{
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 40K    /* SRAM1 40KB */
    CCM (xrw)  : ORIGIN = 0x10000000, LENGTH = 8K     /* CCM RAM 8KB */
    FLASH (rx) : ORIGIN = 0x8000000, LENGTH = 256K    /* Main Flash 256KB */
}

/* Конец стека (используем только основную RAM) */
_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    /* Векторная таблица */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH

    /* Код */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)
        
        KEEP (*(.init))
        KEEP (*(.fini))
        
        . = ALIGN(4);
        _etext = .;
    } >FLASH

    /* Константы */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    /* Инициализированные данные */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT> FLASH

    _sidata = LOADADDR(.data);

    /* Неинициализированные данные */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    /* Куча и стек */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } >RAM

    /* Удаляем информацию отладки */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
}

/* Размеры кучи и стека */
_Min_Heap_Size = 0x200;
_Min_Stack_Size = 0x400;