#include "stm32f103xb.h"
#include <stdint.h>

/* UART functions */
void UART1_Init(void);
void UART1_SendChar(char ch);
void UART1_SendString(char *str);
char UART1_ReadChar(void);

/* GPIO */
void GPIO_Init(void);

/* Delay */
void delay(void);

uint8_t led_state = 0;

int main(void)
{
    UART1_Init();
    GPIO_Init();

    while (1)
    {
        UART1_SendString("\r\n===== LED MENU =====\r\n");
        UART1_SendString("1. Turn ON LED\r\n");
        UART1_SendString("2. Turn OFF LED\r\n");
        UART1_SendString("3. Read LED Status\r\n");
        UART1_SendString("4. Left Traverse\r\n");
        UART1_SendString("5. Right Traverse\r\n");
        UART1_SendString("Enter option: ");

        char choice = UART1_ReadChar();
        UART1_SendChar(choice);

        int led;

        if (choice == '1')
        {
            UART1_SendString("\r\nEnter LED (0-7): ");
            led = UART1_ReadChar() - '0';
            UART1_SendChar(led + '0');

            led_state |= (1 << led);      // OR
            GPIOA->ODR = led_state;
        }

        else if (choice == '2')
        {
            UART1_SendString("\r\nEnter LED (0-7): ");
            led = UART1_ReadChar() - '0';
            UART1_SendChar(led + '0');

            led_state &= ~(1 << led);     // AND + NOT
            GPIOA->ODR = led_state;
        }

        else if (choice == '3')
        {
            UART1_SendString("\r\nEnter LED (0-7): ");
            led = UART1_ReadChar() - '0';
            UART1_SendChar(led + '0');

            if (led_state & (1 << led))   // AND
                UART1_SendString("\r\nLED is ON\r\n");
            else
                UART1_SendString("\r\nLED is OFF\r\n");
        }

        else if (choice == '4')
        {
            UART1_SendString("\r\nLeft Traverse\r\n");
            for (int i = 0; i < 8; i++)
            {
                GPIOA->ODR = (1 << i);   // LEFT SHIFT
                delay();
            }
        }

        else if (choice == '5')
        {
            UART1_SendString("\r\nRight Traverse\r\n");
            for (int i = 7; i >= 0; i--)
            {
                GPIOA->ODR = (1 << i);   // RIGHT SHIFT
                delay();
            }
        }

        else
        {
            UART1_SendString("\r\nInvalid Option\r\n");
        }
    }
}

/* ---------------- GPIO INIT ---------------- */
void GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA0–PA7 → Output push-pull (50 MHz)
    GPIOA->CRL = 0x33333333;
}

/* ---------------- UART INIT ---------------- */
void UART1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // PA9 TX
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0);
    GPIOA->CRH |= GPIO_CRH_CNF9_1;

    // PA10 RX
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF10_0;

    USART1->BRR = 0x45;

    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART1->CR1 |= USART_CR1_UE;
}

/* ---------------- UART TX ---------------- */
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

/* ---------------- UART RX ---------------- */
char UART1_ReadChar(void)
{
    while (!(USART1->SR & USART_SR_RXNE));
    return USART1->DR;
}

/* ---------------- DELAY ---------------- */
void delay(void)
{
    for (volatile int i = 0; i < 500000; i++);
}
