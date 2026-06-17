#include "stm32f103xb.h"
#include <stdint.h>

/* Function Prototypes */
void ADC1_Init(void);
uint16_t ADC1_Read(void);
void GPIO_Init(void);
void delay(void);

/* Threshold (adjust based on your environment) */
#define THRESHOLD 2000   // (0–4095)

int main(void)
{
    ADC1_Init();
    GPIO_Init();

    while (1)
    {
        uint16_t adc_val = ADC1_Read();

        // Dark condition → LED ON
        if (adc_val > THRESHOLD)
        {
            GPIOA->ODR |= (1 << 1);   // PA1 HIGH → LED ON
        }
        else
        {
            GPIOA->ODR &= ~(1 << 1);  // PA1 LOW → LED OFF
        }

        delay();
    }
}

/* ---------------- GPIO INIT ---------------- */
void GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA1 → Output (LED)
    GPIOA->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
    GPIOA->CRL |= (GPIO_CRL_MODE1_1 | GPIO_CRL_MODE1_0); // Output 50MHz
}

/* ---------------- ADC INIT ---------------- */
void ADC1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   // GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;   // ADC1

    // PA0 → Analog mode
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);

    // Turn ON ADC
    ADC1->CR2 |= ADC_CR2_ADON;
    delay();

    // Calibration
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL);
}

/* ---------------- ADC READ ---------------- */
uint16_t ADC1_Read(void)
{
    ADC1->SQR3 = 0;              // Channel 0 (PA0)

    ADC1->CR2 |= ADC_CR2_ADON;   // Start conversion
    delay();

    while (!(ADC1->SR & ADC_SR_EOC)); // Wait for conversion

    return ADC1->DR;
}

/* ---------------- DELAY ---------------- */
void delay(void)
{
    for (volatile int i = 0; i < 50000; i++);
}
