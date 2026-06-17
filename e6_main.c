#include "stm32f103xb.h"

/* -------- FUNCTION DECLARATIONS -------- */
void GPIO_Init(void);
void PWM_Init(void);
void UART_Init(void);
void ADC_Init(void);
uint16_t ADC_Read(void);

char UART_ReadChar(void);
void UART_SendChar(char c);
void UART_SendString(char *str);

void set_pwm_percent(int percent);
void set_pwm_raw(int value);

/* ---------------- GLOBAL ---------------- */
int mode = 0;  // 0 = UART, 1 = POT

/* ---------------- MAIN ---------------- */
int main(void)
{
    GPIO_Init();
    PWM_Init();
    UART_Init();
    ADC_Init();

    UART_SendString("Press U (UART) or P (POT mode)\r\n");

    while (1)
    {
        /* -------- MODE SELECTION -------- */
        if (USART1->SR & USART_SR_RXNE)
        {
            char m = UART_ReadChar();

            if (m == 'P' || m == 'p')
            {
                mode = 1;
                UART_SendString("\r\nPotentiometer Mode\r\n");
            }
            else if (m == 'U' || m == 'u')
            {
                mode = 0;
                UART_SendString("\r\nUART Mode\r\n");
            }
        }

        /* -------- POTENTIOMETER MODE -------- */
        if (mode == 1)
        {
            uint16_t adc_val = ADC_Read();   // 0–4095
            int duty = (adc_val * 1000) / 4095;

            set_pwm_raw(duty);
        }

        /* -------- UART MODE -------- */
        if (mode == 0)
        {
            if (USART1->SR & USART_SR_RXNE)
            {
                char buffer[4];
                int i = 0;
                char c;

                while (1)
                {
                    c = UART_ReadChar();

                    if (c == '\r' || c == '\n')
                        break;

                    if (i < 3)
                        buffer[i++] = c;
                }

                buffer[i] = '\0';

                int value = 0;

                if (i == 2)
                    value = (buffer[0]-'0')*10 + (buffer[1]-'0');
                else if (i == 3)
                    value = (buffer[0]-'0')*100 + (buffer[1]-'0')*10 + (buffer[2]-'0');

                if (value == 25 || value == 50 || value == 75 || value == 100)
                {
                    set_pwm_percent(value);

                    UART_SendString("\r\nSet to: ");
                    UART_SendString(buffer);
                    UART_SendString("%\r\n");
                }
                else
                {
                    UART_SendString("\r\nInvalid input\r\n");
                }
            }
        }
    }
}

/* ---------------- GPIO ---------------- */
void GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /* PA8 → PWM */
    GPIOA->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8);
    GPIOA->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE8_0;
    GPIOA->CRH |= GPIO_CRH_CNF8_1;

    /* PA0 → IN1 */
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    GPIOA->CRL |= GPIO_CRL_MODE0_1 | GPIO_CRL_MODE0_0;

    GPIOA->BSRR = (1 << 0);

    /* PA9 TX */
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0;
    GPIOA->CRH |= GPIO_CRH_CNF9_1;

    /* PA10 RX */
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF10_0;

    /* PA1 → Analog (ADC) */
    GPIOA->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
}

/* ---------------- PWM ---------------- */
void PWM_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    TIM1->PSC = 8 - 1;
    TIM1->ARR = 1000 - 1;

    TIM1->CCMR1 |= (6 << 4);
    TIM1->CCMR1 |= (1 << 3);

    TIM1->CCER |= TIM_CCER_CC1E;
    TIM1->BDTR |= TIM_BDTR_MOE;

    TIM1->CR1 |= TIM_CR1_CEN;
}

/* ---------------- ADC ---------------- */
void ADC_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CR2 |= ADC_CR2_ADON;
    for (volatile int i = 0; i < 1000; i++);

    ADC1->SQR3 = 1;  // channel 1 (PA1)
}

uint16_t ADC_Read(void)
{
    ADC1->CR2 |= ADC_CR2_ADON;
    while (!(ADC1->SR & ADC_SR_EOC));

    return ADC1->DR;
}

/* ---------------- UART ---------------- */
void UART_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    USART1->BRR = 0x341;

    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART1->CR1 |= USART_CR1_UE;
}

char UART_ReadChar(void)
{
    while (!(USART1->SR & USART_SR_RXNE));

    char c = USART1->DR;
    UART_SendChar(c);
    return c;
}

void UART_SendChar(char c)
{
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}

void UART_SendString(char *str)
{
    while (*str)
        UART_SendChar(*str++);
}

/* ---------------- PWM CONTROL ---------------- */
void set_pwm_percent(int percent)
{
    int duty = (percent * 1000) / 100;
    TIM1->CCR1 = duty;
}

void set_pwm_raw(int value)
{
    TIM1->CCR1 = value;
}
