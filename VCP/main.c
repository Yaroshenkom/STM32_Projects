#include "stm32f4xx.h"
#include "MyUSART.h"
#include "I2C.h"

int main()
{
	InitUSART2();
	/*I2C1Initialize();
	InitialzieI2C_PB6_PB7();
	I2C1LCDInitialization();*/
	/*while(1){
	while(!(USART2->SR & USART_SR_RXNE)){}
	I2C1LCDSendByte(USART2->DR, 1);
	}*/
	while(1)
	{
		USART2->DR = 'A';
		
		for(int i=0; i < 1000; i++) TIM2Delay(1000);
	}
}