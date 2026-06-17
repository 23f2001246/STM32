#include "stm32f103xb.h"

void UART1_Init(void);
void UART1_SendChar(char ch);
void UART1_SendString(char *str);
void delay(void);

int main(void)
{
    UART1_Init();

    UART1_SendString("Hello Riser\r\n");

    while (1);
}

/* ---------------- UART INIT ---------------- */
void UART1_Init(void)
{
    /* 1. Enable Clocks */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   // GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // USART1 clock

    /* 2. Configure PA9 (TX) as Alternate Function Push-Pull */
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);  // Clear
    GPIOA->CRH |=  (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0); // Output 50 MHz
    GPIOA->CRH |=  GPIO_CRH_CNF9_1; // AF Push-Pull

    /* 3. Set Baud Rate = 115200 (8 MHz clock) */
    USART1->BRR = 0x45;  // Pre-calculated

    /* 4. Enable USART and Transmitter */
    USART1->CR1 |= USART_CR1_TE;  // Enable TX
    USART1->CR1 |= USART_CR1_UE;  // Enable USART
}

/* ---------------- SEND CHAR ---------------- */
void UART1_SendChar(char ch)
{
    while (!(USART1->SR & USART_SR_TXE)); // Wait until TX empty
    USART1->DR = ch;
}

/* ---------------- SEND STRING ---------------- */
void UART1_SendString(char *str)
{
    while (*str)
    {
        UART1_SendChar(*str++);
    }
}

/* ---------------- DELAY ---------------- */
void delay(void)
{
    for (volatile int i = 0; i < 500000; i++);
}
