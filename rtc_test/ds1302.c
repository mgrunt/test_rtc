#include "ds1302.h"
#include "delay.h"

#define Ds1302_Charge() Ds1302_Writeoneword(0x90,0xa4)
#define Ds1302_ENWrite() Ds1302_Writeoneword(0x8e,0x00)
#define Ds1302_DISWrite() Ds1302_Writeoneword(0x8e,0x80)

ds1302_time time;

uint8_t ChangeTohex(uint8_t dat)
{
	uint8_t temp;
	temp = (dat/10)*16+dat%10;
	return temp;
}



uint8_t ChangeToDec(uint8_t dat)
{
	uint8_t temp;
	temp = (dat/16)*10+dat%16;
	return temp;
}


void Ds1302_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	delay_init(48);

	//SCLK
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS1302_PORT, &GPIO_InitStructure);

	//CE
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//|GPIO_Pin_5; PC4--DS18B20_CE PC5--DS18B20_IO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(DS1302_PORT, &GPIO_InitStructure);
}

void Ds1302_Writeonechar(uint8_t m)
{
	uint8_t i;

	for(i=0; i<8; i++)
	{
		DS1302_SCLK(0);
		delay_us(30);
		if(m&0x01==1)
		{
			DS1302_OUTPUT(1);
		}
		else
		{
			DS1302_OUTPUT(0);
		}
		delay_us(30);
		DS1302_SCLK(1);
		delay_us(30);
		m = m>>1;
		delay_ms(1);
	}
}

uint8_t Ds1302_Readonechar()
{
	uint8_t i, temp = 0x00;

	for(i=0; i<8; i++)
	{
		temp = temp>>1;
		DS1302_SCLK(0);
		delay_us(30);
		if(DS1302_INPUT()==1)
			temp = temp|0x80;
		DS1302_SCLK(1);
		delay_us(30);
	}
	return temp;
}

uint8_t Ds1302_Readoneword(uint8_t address)
{
	uint8_t temp;
	GPIO_InitTypeDef GPIO_InitStructure;

	DS1302_SCLK(0);
	delay_us(30);
	DS1302_CE(1);
	delay_us(300);

	GPIO_InitStructure.GPIO_Pin = DS1302_INPUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS1302_PORT, &GPIO_InitStructure);

	Ds1302_Writeonechar(address);

	GPIO_InitStructure.GPIO_Pin = DS1302_INPUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(DS1302_PORT, &GPIO_InitStructure);

	temp = Ds1302_Readonechar();

	delay_us(30);
	DS1302_SCLK(0);
	delay_us(30);
	DS1302_CE(0);

	return temp;
}


void Ds1302_Writeoneword(uint8_t address,uint8_t shuju)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	DS1302_SCLK(0);
	delay_us(30);
	DS1302_CE(1);

	GPIO_InitStructure.GPIO_Pin = DS1302_INPUT_PIN;//PB0 --DS18B20_SCLK
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS1302_PORT, &GPIO_InitStructure);

	Ds1302_Writeonechar(address);
	Ds1302_Writeonechar(shuju);

	DS1302_SCLK(0);
	delay_us(30);
	DS1302_CE(0);
}

void Ds1302_Settime(uint16_t year, uint8_t month, uint8_t day, uint8_t week, uint8_t hour, uint8_t minute)
{
	year = ChangeTohex(year-2000);
	month = ChangeTohex(month);
	day = ChangeTohex(day);
	hour = ChangeTohex(hour);
	minute = ChangeTohex(minute);
	week = ChangeTohex(week);

	Ds1302_ENWrite();

	Ds1302_Writeoneword(0x8c,year);
	Ds1302_Writeoneword(0x88,month);
	Ds1302_Writeoneword(0x86,day);
	Ds1302_Writeoneword(0x8a,week);
	Ds1302_Writeoneword(0x84,hour);
	Ds1302_Writeoneword(0x82,minute);
	Ds1302_Writeoneword(0x80,0x00);

	Ds1302_DISWrite();
}

void Ds1302_Gettime(void)
{
	uint8_t buffer[7];

	buffer[6] = Ds1302_Readoneword(0x8d);
	buffer[5] = Ds1302_Readoneword(0x89)&0x1f;
	buffer[4] = Ds1302_Readoneword(0x87)&0x3f;
	buffer[3] = Ds1302_Readoneword(0x8b)&0x3f;
	buffer[2] = Ds1302_Readoneword(0x85)&0x1f;
	buffer[1] = Ds1302_Readoneword(0x83)&0x7f;
	buffer[0] = Ds1302_Readoneword(0x81)&0x7f;

	time.w_year = ChangeToDec(buffer[6])+2000;
	time.w_month = ChangeToDec(buffer[5]);
	time.w_date = ChangeToDec(buffer[4]);
	time.week = ChangeToDec(buffer[3]);
	time.hour = ChangeToDec(buffer[2]);
	time.min = ChangeToDec(buffer[1]);
	time.sec = ChangeToDec(buffer[0]);
}
