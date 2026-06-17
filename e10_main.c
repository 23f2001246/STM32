#include "stm32f103xb.h"

/* Pin Definitions */
#define RS_HIGH()  GPIOA->BSRR = (1<<0)
#define RS_LOW()   GPIOA->BRR  = (1<<0)

#define EN_HIGH()  GPIOA->BSRR = (1<<1)
#define EN_LOW()   GPIOA->BRR  = (1<<1)

/* Function Prototypes */
void GPIO_Init(void);
void LCD_Init(void);
void LCD_Command(uint8_t cmd);
void LCD_Data(uint8_t data);
void LCD_String(char *str);
void LCD_Send4Bit(uint8_t data);
void delay(void);

/* ---------------- MAIN ---------------- */
int main(void)
{
    GPIO_Init();
    LCD_Init();

    LCD_Command(0x80);          // First line
    LCD_String("ASWATHI G");

    LCD_Command(0xC0);          // Second line
    LCD_String("SSNCE");

    while(1);
}

/* ---------------- GPIO INIT ---------------- */
void GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA0–PA5 as output
    GPIOA->CRL &= ~(0xFFFFFF);
    GPIOA->CRL |=  (0x333333);
}

/* ---------------- LCD INIT ---------------- */
void LCD_Init(void)
{
    delay();

    LCD_Send4Bit(0x03);
    delay();
    LCD_Send4Bit(0x03);
    delay();
    LCD_Send4Bit(0x03);
    delay();
    LCD_Send4Bit(0x02);   // 4-bit mode

    LCD_Command(0x28);    // 4-bit, 2 line
    LCD_Command(0x0C);    // Display ON
    LCD_Command(0x06);    // Entry mode
    LCD_Command(0x01);    // Clear display
}

/* ---------------- SEND COMMAND ---------------- */
void LCD_Command(uint8_t cmd)
{
    RS_LOW();

    LCD_Send4Bit(cmd >> 4);
    LCD_Send4Bit(cmd);

    delay();
}

/* ---------------- SEND DATA ---------------- */
void LCD_Data(uint8_t data)
{
    RS_HIGH();

    LCD_Send4Bit(data >> 4);
    LCD_Send4Bit(data);

    delay();
}

/* ---------------- SEND 4 BIT ---------------- */
void LCD_Send4Bit(uint8_t data)
{
    // Clear D4–D7 (PA2–PA5)
    GPIOA->BRR = (0xF << 2);

    // Set data
    GPIOA->BSRR = ((data & 0x0F) << 2);

    EN_HIGH();
    delay();
    EN_LOW();
}

/* ---------------- STRING ---------------- */
void LCD_String(char *str)
{
    while (*str)
    {
        LCD_Data(*str++);
    }
}

/* ---------------- DELAY ---------------- */
void delay(void)
{
    for (volatile int i = 0; i < 2000; i++);
}
