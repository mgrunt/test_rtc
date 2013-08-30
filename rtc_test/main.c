#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"
#include "delay.h"
#include "ds1302.h"

int main(void)
{
	ds1302_time time;

	Ds1302_Init();

	// year, month, day, week, hour, minute
	Ds1302_Settime(2013,8,29,35,20,00);

	Ds1302_Gettime();

	//time.hour

	return 0;
}
