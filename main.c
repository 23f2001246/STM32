#include "stm32f103xb.h"
#include <stdint.h>

/* UART Functions */
void UART1_Init(void);
void UART1_SendChar(char ch);
void UART1_SendString(char *str);
char UART1_ReadChar(void);

/* Utility */
uint32_t read_hex(void);
void print_hex(uint32_t val);

int main(void)
{
    UART1_Init();

    while (1)
    {
        UART1_SendString("\r\n===== MENU =====\r\n");
        UART1_SendString("1. Read Register\r\n");
        UART1_SendString("2. Write Register\r\n");
        UART1_SendString("Enter option: ");

        char choice = UART1_ReadChar();
        UART1_SendChar(choice); // echo

        if (choice == '1')
        {
            UART1_SendString("\r\nEnter address (hex): ");
            uint32_t addr = read_hex();

            uint32_t value = *(volatile uint32_t*)addr;

            UART1_SendString("\r\nValue: ");
            print_hex(value);
        }
        else if (choice == '2')
        {
            UART1_SendString("\r\nEnter address (hex): ");
            uint32_t addr = read_hex();

            UART1_SendString("\r\nEnter value (hex): ");
            uint32_t val = read_hex();

            *(volatile uint32_t*)addr = val;

            UART1_SendString("\r\nWrite Done!\r\n");
        }
        else
        {
            UART1_SendString("\r\nInvalid Option\r\n");
        }
    }
}

/* ---------------- UART INIT ---------------- */
void UART1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0);
    GPIOA->CRH |= GPIO_CRH_CNF9_1;

    // RX (PA10)
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF10_0; // input floating

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

/* ---------------- READ HEX INPUT ---------------- */
uint32_t read_hex(void)
{
    char c;
    uint32_t val = 0;

    while (1)
    {
        c = UART1_ReadChar();

        if (c == '\r') break;

        UART1_SendChar(c); // echo

        if (c >= '0' && c <= '9')
            val = (val << 4) | (c - '0');
        else if (c >= 'A' && c <= 'F')
            val = (val << 4) | (c - 'A' + 10);
        else if (c >= 'a' && c <= 'f')
            val = (val << 4) | (c - 'a' + 10);
    }

    return val;
}

/* ---------------- PRINT HEX ---------------- */
void print_hex(uint32_t val)
{
    char hex[] = "0123456789ABCDEF";

    for (int i = 28; i >= 0; i -= 4)
    {
        UART1_SendChar(hex[(val >> i) & 0xF]);
    }
    UART1_SendString("\r\n");
}
