#include "string.h"
#include "I2C.h"
#include "stm32f4xx.h"

void InitialzieI2C_PB6_PB7()
{
	//Enable the GPIOB
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	//Set alternative function mode AF for PB6-7
	GPIOB->MODER &= ~GPIO_MODER_MODE6_0;
	GPIOB->MODER |= GPIO_MODER_MODE6_1;
	GPIOB->MODER &= ~GPIO_MODER_MODE7_0;
	GPIOB->MODER |= GPIO_MODER_MODE7_1;
	
	//Set type register: open drain
	GPIOB->OTYPER |= GPIO_OTYPER_OT_6;
	GPIOB->OTYPER |= GPIO_OTYPER_OT_7;
	
	//Set the lowest speed for PB6-7
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR6_0;
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR7_0;
	
	//No internal pull-up or pull-down resistors, just open-drain mode
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD6;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD7;
	
	//Set the number for AF's of PB6-7: PB6 - SCL, PB7 - SDA
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFRL6;
	GPIOB->AFR[0] |= GPIO_AFRL_AFRL6_2;
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFRL7;
	GPIOB->AFR[0] |= GPIO_AFRL_AFRL7_2;
}


void I2C1Initialize()
{
	//Enable the I2C1
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	
	//Reset the I2C1 to fix the busy flag issue
	RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
	while (I2C1->SR1 & I2C_SR2_BUSY){}
	RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
	
	//Set CLK for I2C1 module - 2MHz
	I2C1->CR2 &= ~I2C_CR2_FREQ;
	I2C1->CR2 |= 2;
	
	//Set prescaler to 8MHz/100kHz=80 for data exchange
	I2C1->CCR &= ~I2C_CCR_CCR;
	I2C1->CCR |= 40;
	
	//Set maximum rise speed in tacts 1000ns/(1/2MHz)=2 +1=3
	I2C1->TRISE = 3;	
}



void I2C1LCDInitialization()
{
	//Set the working conditions of the screen
	TIM2Delay(40000);
	I2C1LCDSendNibble(0b00110000);
	TIM2Delay(4000);
	I2C1LCDSendNibble(0b00110000);
	TIM2Delay(60);
	I2C1LCDSendNibble(0b00110000);
	TIM2Delay(60);
	I2C1LCDSendNibble(0b00100000);
	TIM2Delay(60);
	//Firstly, set the 4-bit interface 2 strings of 5x8 size
	I2C1LCDSendByte(0b00101000, 0);
	TIM2Delay(60);
	//Thirdly, set the cursor
	I2C1LCDSendByte(0b00001100, 0);
	TIM2Delay(60);
	//Secondly, clear the screen
	I2C1LCDSendByte(0b00000001, 0);
	TIM2Delay(2000);
	//Fourthly, let's make the cursor movable, not the screen
	I2C1LCDSendByte(0b00000110, 0);
	TIM2Delay(60);
}

void I2C1LCDSendByte(int byte, int isdata)
{
	int mst, lst;
	mst = byte & 0b11110000;
	lst = (byte & 0b00001111) << 4;
	//Set the LED bit
	mst |= 8;
	lst |= 8;
	//If we send data, we have to set the RS bit
	if(isdata)
	{
		mst |= 1;
		lst |= 1;
	}
	//Allow work of the I2C1 module
	I2C1->CR1 |= I2C_CR1_PE;	
	//Make a "start" signal
	I2C1->CR1 |= I2C_CR1_START;
	//Wait for the end of the "start" signal
	while (!(I2C1->SR1 & I2C_SR1_SB)){}	
	//To reset the SB bit read the SR1 register
	(void) I2C1->SR1;	
	//Send an LCD address - 0x7E for write, 0x7F for read
	I2C1->DR = 0x7E;		
	//Wait for the end of address sending
	while (!(I2C1->SR1 & I2C_SR1_ADDR)){}
	//Read SR1 and SR2 to clear the ADDR bit
	(void) I2C1->SR1;
	(void) I2C1->SR2;		
	//Send the elder 4 bits with E=1
	mst |= 4;
	I2C1->DR=mst;
	//Wait for the finish of byte transmission
	while (!(I2C1->SR1 & I2C_SR1_BTF)){}
	//Send the elder 4 bits with E=0
	mst &= ~4;
	I2C1->DR=mst;
	while (!(I2C1->SR1 & I2C_SR1_BTF)){}
	//Send the younger 4 bits with E=1
	lst |= 4;
	I2C1->DR=lst;
	while (!(I2C1->SR1 & I2C_SR1_BTF)){}
	//Send the younger 4 bits with E=0
	lst &= ~4;
	I2C1->DR=lst;
	while (!(I2C1->SR1 & I2C_SR1_BTF)){}
	//Set the stop signal
	I2C1->CR1 |= I2C_CR1_STOP;
	//Stop the I2C1 work
	I2C1->CR1 &= ~I2C_CR1_PE;	
}

void TIM2Delay(int us)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 8;
	TIM2->ARR = us;
	TIM2->CR1 |= TIM_CR1_CEN;
	while(!(TIM2->SR & TIM_SR_UIF)){}
	TIM2->CR1 &= ~TIM_CR1_CEN;
	TIM2->SR &= ~TIM_SR_UIF;
	RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
}


void I2C1LCDSendNibble(int nibble)
{
	//Allow work of the I2C1 module
	I2C1->CR1 |= I2C_CR1_PE;	
	//Make a "start" signal
	I2C1->CR1 |= I2C_CR1_START;
	//Wait for the end of the "start" signal
	while (!(I2C1->SR1 & I2C_SR1_SB)){}	
	//To reset the SB bit read the SR1 register
	(void) I2C1->SR1;
	//Send an LCD address - 0x7E for write, 0x7F for read
	I2C1->DR = 0x7E;		
	//Wait for the end of address sending
	while (!(I2C1->SR1 & I2C_SR1_ADDR)){}
	//Read SR1 and SR2 to clear the ADDR bit
	(void) I2C1->SR1;
	(void) I2C1->SR2;
	//Send the elder 4 bits with E=1
	nibble |= 4;
	I2C1->DR=nibble;
	while (!(I2C1->SR1 & I2C_SR1_BTF)){}
	//Send the elder 4 bits with E=0
	nibble &= ~4;
	I2C1->DR=nibble;
	while (!(I2C1->SR1 & I2C_SR1_BTF)){}
	//Set the stop signal
	I2C1->CR1 |= I2C_CR1_STOP;
	//Stop the I2C1 work
	I2C1->CR1 &= ~I2C_CR1_PE;	
}


void I2C1SendString(char *str)
{
	int sizeofstring;
	sizeofstring = strlen(str);
	if(sizeofstring > 16) sizeofstring = 16;
	for(int i=1; i <= sizeofstring; i++)
	{
		I2C1LCDSendByte(str[i-1],1);
		TIM2Delay(60);
	}
}
/*void I2C1LCDWaitForBusyFlagReset()
{
	int tmp;
	//Allow work of the I2C1 module
	I2C1->CR1 |= I2C_CR1_PE;
	do
	{
		//Make a "start" signal
		I2C1->CR1 |= I2C_CR1_START;
		//Wait for the end of the "start" signal
		while (!(I2C1->SR1 & I2C_SR1_SB)){}	
		//To reset the SB bit read the SR1 register
		(void) I2C1->SR1;
		//Send an LCD address - 0x7E for write, 0x7F for read
		I2C1->DR = 0x7E;
		//Read SR1 and SR2 to clear the ADDR bit
	(void) I2C1->SR1;
	(void) I2C1->SR2;	
			
	//Send the elder 4 bits with E=1
	I2C1->DR=0b00001110;
	//Wait for the finish of byte transmission
	while (!(I2C1->SR1 & I2C_SR1_BTF)){}
	//Send the elder 4 bits with E=0
	I2C1->DR=0b00001010;
	while (!(I2C1->SR1 & I2C_SR1_BTF)){}
	//Send the younger 4 bits with E=1
	I2C1->DR=0b00001110;
	while (!(I2C1->SR1 & I2C_SR1_BTF)){}
	//Send the younger 4 bits with E=0
	I2C1->DR=0b00001010;
	while (!(I2C1->SR1 & I2C_SR1_BTF)){}
		
		//Make a repetitive "start" signal
		I2C1->CR1 |= I2C_CR1_START;
		//Wait for the end of the "start" signal
		while (!(I2C1->SR1 & I2C_SR1_SB)){}	
		//To reset the SB bit read the SR1 register
		(void) I2C1->SR1;
		//Send an LCD address - 0x7E for write, 0x7F for read
		I2C1->DR = 0x7F;
		//Read SR1 and SR2 to clear the ADDR bit
	(void) I2C1->SR1;
	(void) I2C1->SR2;	
			
		//Wait until the reading ends
		while (!(I2C1->SR1 & I2C_SR1_RXNE)){}
		//Set the stop signal
		I2C1->CR1 |= I2C_CR1_STOP;
		//Check the LCD's busy flag
		tmp = I2C1->DR;
	}while(tmp & 0b10000000);
		//Stop the I2C1 work
		I2C1->CR1 &= ~I2C_CR1_PE;	
}

void I2C1FixBusyFlagIssue()
{
	//1. Disable the I2C peripheral by clearing the PE bit in I2Cx_CR1 register
	I2C1->CR1 &= ~I2C_CR1_PE;
	
	//2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
	GPIOB->MODER &= ~GPIO_MODER_MODE6_1;
	GPIOB->MODER |= GPIO_MODER_MODE6_0;
	GPIOB->MODER &= ~GPIO_MODER_MODE7_1;
	GPIOB->MODER |= GPIO_MODER_MODE7_0;
	GPIOB->OTYPER |= GPIO_OTYPER_OT_6;
	GPIOB->OTYPER |= GPIO_OTYPER_OT_7;
	GPIOB->ODR |= GPIO_ODR_OD6 | GPIO_ODR_OD7;
	
	//3. Check SCL and SDA High level in GPIOx_IDR
	while(!(GPIOB->IDR & (GPIO_IDR_ID6 | GPIO_IDR_ID7))){}
		
	//4. Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
	GPIOB->ODR &= ~GPIO_ODR_OD7;
	
	//5. Check SDA Low level in GPIOx_IDR
	while(GPIOB->IDR & GPIO_IDR_ID7){}
		
	//6. Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
	GPIOB->ODR &= ~GPIO_ODR_OD6;
		
	//7. Check SCL Low level in GPIOx_IDR.
	while(GPIOB->IDR & GPIO_IDR_ID6){}
		
	//8. Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
	GPIOB->ODR |= GPIO_ODR_OD6;
		
	//9. Check SCL High level in GPIOx_IDR.
	while(!(GPIOB->IDR & GPIO_IDR_ID6)){}
		
	//10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to GPIOx_ODR).
	GPIOB->ODR |= GPIO_ODR_OD7;
		
	//11. Check SDA High level in GPIOx_IDR.
	while(!(GPIOB->IDR & GPIO_IDR_ID7)){}
	
	//12. Configure the SCL and SDA I/Os as Alternate function Open-Drain.
	GPIOB->MODER &= ~GPIO_MODER_MODE6_0;
	GPIOB->MODER |= GPIO_MODER_MODE6_1;
	GPIOB->MODER &= ~GPIO_MODER_MODE7_0;
	GPIOB->MODER |= GPIO_MODER_MODE7_1;
	
	//13. Set SWRST bit in I2Cx_CR1 register.
	I2C1->CR1 |= I2C_CR1_SWRST;
		
	//14. Clear SWRST bit in I2Cx_CR1 register.
	I2C1->CR1 &= ~I2C_CR1_SWRST;
	
	//15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register.
	I2C1->CR1 |= I2C_CR1_PE;
}*/
	