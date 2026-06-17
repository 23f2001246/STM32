#include "stm32f103xb.h"

/* Function Prototypes */
void UART1_Init(void);
void UART1_SendChar(char ch);
void UART1_SendString(char *str);

void GPIO_Init(void);
char Keypad_Scan(void);
void delay(void);

/* Keypad Layout */
char keypad[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

/* ---------------- MAIN ---------------- */
int main(void)
{
    UART1_Init();
    GPIO_Init();

    while (1)
    {
        char key = Keypad_Scan();

        if (key != 0)
        {
            UART1_SendString("\r\nKey Pressed: ");
            UART1_SendChar(key);
            UART1_SendString("\r\n");

            delay(); // debounce
            while (Keypad_Scan() != 0); // wait until key release
        }
    }
}

/* ---------------- GPIO INIT ---------------- */
void GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    /* ROWS (PA0–PA3) → OUTPUT */
    GPIOA->CRL &= ~(0xFFFF);
    GPIOA->CRL |=  (0x3333);   // output push-pull

    /* COLUMNS (PB0, PB1, PB10, PB11) → INPUT PULL-UP */

    // PB0, PB1
    GPIOB->CRL &= ~((0xF << 0) | (0xF << 4));
    GPIOB->CRL |=  ((0x8 << 0) | (0x8 << 4));

    // PB10, PB11
    GPIOB->CRH &= ~((0xF << 8) | (0xF << 12));
    GPIOB->CRH |=  ((0x8 << 8) | (0x8 << 12));

    // Enable pull-up
    GPIOB->ODR |= (1<<0) | (1<<1) | (1<<10) | (1<<11);

    // Set all rows HIGH initially
    GPIOA->ODR |= 0x0F;
}

/* ---------------- KEYPAD SCAN ---------------- */
char Keypad_Scan(void)
{
    for (int row = 0; row < 4; row++)
    {
        // Set all rows HIGH
        GPIOA->ODR = (GPIOA->ODR & ~0x0F) | 0x0F;

        // Set current row LOW
        GPIOA->ODR &= ~(1 << row);

        delay(); // small settling delay

        // Check columns (active LOW)
        if (!(GPIOB->IDR & (1<<0))) return keypad[row][0];
        if (!(GPIOB->IDR & (1<<1))) return keypad[row][1];
        if (!(GPIOB->IDR & (1<<10))) return keypad[row][2];
        if (!(GPIOB->IDR & (1<<11))) return keypad[row][3];
    }

    return 0;
}

/* ---------------- UART INIT ---------------- */
void UART1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // PA9 → TX
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0;
    GPIOA->CRH |= GPIO_CRH_CNF9_1;

    USART1->BRR = 0x45;

    USART1->CR1 |= USART_CR1_TE;
    USART1->CR1 |= USART_CR1_UE;
}

/* ---------------- UART SEND ---------------- */
void UART1_SendChar(char ch)
{
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = ch;
}

void UART1_SendString(char *str)
{
    while (*str)
        UART1_SendChar(*str++);
}

/* ---------------- DELAY ---------------- */
void delay(void)
{
    for (volatile int i = 0; i < 30000; i++);
}
