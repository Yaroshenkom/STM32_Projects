#include "MyUSART.h"
#include "stm32f4xx.h"
#include <stdlib.h>

#define BRR19200(f) ((div(f/19200, 16).quot)<<4)|(div(f/19200, 16).rem)

void InitUSART2()
{
	//Set the SYSCLK to 40 MHz
	InitSysCLK40MHz();
	
	//Enable the USART2 module
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	
	//Set the USART2 baudrate to 19200
	USART2->BRR = 0x823;
	
	//Turn on transmission (TX) and recieving (RX)
	USART2->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
	
	//Now, configure PA2-3 as a TX and RX
	//Enable PA
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	//Set the PA2-3 to the AF mode
	GPIOA->MODER |= GPIO_MODER_MODE2 | GPIO_MODER_MODE3;
	GPIOA->MODER &= (~GPIO_MODER_MODE2_0) & (~GPIO_MODER_MODE3_0);
	
	
	//Set the PA2-3 alternate functions
	GPIOA->AFR[0] |= GPIO_AFRL_AFRL2 | GPIO_AFRL_AFRL3;
	GPIOA->AFR[0] &= (~GPIO_AFRL_AFRL2_3) & (~GPIO_AFRL_AFRL3_3);
}

void InitSysCLK40MHz(void)
{
	//Set the HSI/2 as a source for the PLL
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
	
	//Set the PLL multiplication to 10 to get SYSCLK=40 MHz
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLN & (320 << 6));
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;
	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLP & (3 << 16));
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLM & 16);
	
	
	//Turn the PLL on
	RCC->CR |= RCC_CR_PLLON;
	
	//Set the PLL as a source for SYSCLK
	RCC->CFGR |= (RCC_CFGR_SW & 2);
}