#include "stm32f4xx.h"
#include "I2C.h"

int main(void)
{
	char *str1, *str2;
	str1 = "________________";
	str2 = "Test string@`~%*";
	InitialzieI2C_PB6_PB7();
	I2C1Initialize();
	I2C1LCDInitialization();
	I2C1SendString(str1);
	I2C1LCDSendByte(0b11000000,0);
	TIM2Delay(60);
	I2C1SendString(str2);
	while(1){}
}