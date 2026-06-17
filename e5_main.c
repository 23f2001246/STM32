#include "stm32f103xb.h"
#include <stdint.h>

void ADC1_Init(void);
uint16_t ADC1_Read(void);

void PWM_Init(void);
void delay(void);

int main(void)
{
    ADC1_Init();
    PWM_Init();

    while (1)
    {
        uint16_t adc_val = ADC1_Read();   // 0–4095

        // Map ADC (12-bit) → PWM (0–999)
        uint16_t pwm_val = ((4095 - adc_val) * 1000) / 4095;

        TIM1->CCR1 = pwm_val;  // Update duty cycle

        delay();
    }
}

/* ---------------- ADC INIT (PA0) ---------------- */
void ADC1_Init(void)
{
    // Enable clocks
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   // GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;   // ADC1

    // PA0 → Analog mode
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);

    // ADC configuration
    ADC1->CR2 |= ADC_CR2_ADON;   // Power ON
    delay();

    ADC1->CR2 |= ADC_CR2_CAL;    // Calibration
    while (ADC1->CR2 & ADC_CR2_CAL);
}

/* ---------------- ADC READ ---------------- */
uint16_t ADC1_Read(void)
{
    ADC1->SQR3 = 0;              // Channel 0 (PA0)

    ADC1->CR2 |= ADC_CR2_ADON;   // Start conversion
    delay();

    while (!(ADC1->SR & ADC_SR_EOC)); // Wait

    return ADC1->DR;
}

/* ---------------- PWM INIT (PA8 - TIM1 CH1) ---------------- */
void PWM_Init(void)
{
    // Enable clocks
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   // GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;   // TIM1

    // PA8 → Alternate function push-pull
    GPIOA->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8);
    GPIOA->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE8_0; // 50 MHz
    GPIOA->CRH |= GPIO_CRH_CNF8_1; // AF push-pull

    // Timer config
    TIM1->PSC = 8 - 1;      // Prescaler → 1 MHz (8MHz / 8)
    TIM1->ARR = 1000 - 1;   // Period → 1 kHz PWM

    // PWM mode
    TIM1->CCMR1 |= (6 << 4);  // PWM mode 1
    TIM1->CCMR1 |= (1 << 3);  // Preload enable

    TIM1->CCER |= TIM_CCER_CC1E; // Enable channel

    TIM1->CCR1 = 0; // Start OFF

    TIM1->BDTR |= TIM_BDTR_MOE; // Main output enable
    TIM1->CR1 |= TIM_CR1_CEN;   // Start timer
}

/* ---------------- DELAY ---------------- */
void delay(void)
{
    for (volatile int i = 0; i < 10000; i++);
}
