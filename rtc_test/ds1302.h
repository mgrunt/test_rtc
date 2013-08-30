#ifndef _Ds1302_H
#define _Ds1302_H

#include "stm32f0xx.h"

#define DS1302_PORT			GPIOB

#define DS1302_SCLK_PIN		GPIO_Pin_0
#define DS1302_OUTPUT_PIN	GPIO_Pin_5
#define DS1302_CE_PIN		GPIO_Pin_4


#define DS1302_INPUT_PIN	DS1302_OUTPUT_PIN

#define DS1302_SCLK(x) (x? GPIO_SetBits(DS1302_PORT, DS1302_SCLK_PIN): GPIO_ResetBits(DS1302_PORT, DS1302_SCLK_PIN))
#define DS1302_OUTPUT(x) (x? GPIO_SetBits(DS1302_PORT, DS1302_OUTPUT_PIN): GPIO_ResetBits(DS1302_PORT, DS1302_OUTPUT_PIN))
#define DS1302_CE(x) (x? GPIO_SetBits(DS1302_PORT, DS1302_CE_PIN): GPIO_ResetBits(DS1302_PORT, DS1302_CE_PIN))

#define DS1302_INPUT() GPIO_ReadInputDataBit(DS1302_PORT, DS1302_INPUT_PIN)

typedef struct
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;

	uint16_t w_year;
	uint8_t w_month;
	uint8_t w_date;
	uint8_t week;
} ds1302_time;

void Ds1302_Init(void);
void Ds1302_Writeoneword(uint8_t address, uint8_t shuju);
uint8_t Ds1302_Readoneword(uint8_t);
void Ds1302_Gettime(void);
void Ds1302_Settime(uint16_t year, uint8_t month, uint8_t day, uint8_t week, uint8_t hour, uint8_t minute);

#endif
