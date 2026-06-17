#include "stm32f103xb.h"

/* Function Prototypes */
void UART1_Init(void);
void UART1_SendChar(char ch);
void UART1_SendString(char *str);
char UART1_ReadChar(void);

void GPIO_Init(void);

/* ---------------- MAIN ---------------- */
int main(void)
{
    UART1_Init();
    GPIO_Init();

    while (1)
    {
        UART1_SendString("\r\n===== RELAY MENU =====\r\n");
        UART1_SendString("1. Relay ON\r\n");
        UART1_SendString("2. Relay OFF\r\n");
        UART1_SendString("Enter option: ");

        char choice = UART1_ReadChar();
        UART1_SendChar(choice);

        if (choice == '1')
        {
            // ACTIVE LOW → ON
            GPIOA->ODR &= ~(1 << 0);
            UART1_SendString("\r\nRelay ON\r\n");
        }
        else if (choice == '2')
        {
            // ACTIVE LOW → OFF
            GPIOA->ODR |= (1 << 0);
            UART1_SendString("\r\nRelay OFF\r\n");
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

    // PA0 → Output push-pull (50 MHz)
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    GPIOA->CRL |= (GPIO_CRL_MODE0_1 | GPIO_CRL_MODE0_0); // Output 50MHz
}

/* ---------------- UART INIT ---------------- */
void UART1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // PA9 → TX
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0);
    GPIOA->CRH |= GPIO_CRH_CNF9_1;

    // PA10 → RX
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF10_0;

    USART1->BRR = 0x45;

    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
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

/* ---------------- UART READ ---------------- */
char UART1_ReadChar(void)
{
    while (!(USART1->SR & USART_SR_RXNE));
    return USART1->DR;
}
