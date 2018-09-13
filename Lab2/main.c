typedef unsigned long uint32;
typedef unsigned uint16;

#define	RCC_APB1RAPB_ENABLE_REG (uint32*) 0x40023840
#define RCC_APB1RAPB_TIM2EN_OFFSET 0
#define RCC_APB1RAPB_TIM3EN_OFFSET 1

#define TIM2_BASE_OFFSET (uint32*) 0x40000000
#define TIM3_BASE_OFFSET (uint32*) 0x40000400

#define	DIER_REG_OFFSET 0xC
#define DIER_UIE_BIT_OFFSET 0

#define	PSC_REG_OFFSET 0x28

#define	ARR_REG_OFFSET 0x2C
#define ARR_TIM2 1000
#define ARR_TIM3 1

#define CR1_REG_OFFSET 0x0
#define CR1_CEN_OFFSET 0

#define PSC_REG_OFFSET 0x28
#define TIM2_PSC 7999

#define CR2_REG_OFFSET 4
#define CR2_MMS_BIT_OFFSET (6-2) //from 4 to 6 - 3 bites
#define CR2_MMS_MASTER_MODE 2 //010b - The update event is selected as trigger output

#define SMCR_REG_OFFSET 8
#define SMCR_SMS_BIT_OFFSET 0
#define SMCR_SMS_SLAVE_MODE 7 //111b -  External Clock Mode 1 - Rising edges of the selected trigger (TRGI) clock the counter
#define SMCR_TS_BIT_OFFSET (6-2)
#define SMCR_TS_TI1_INPUT 1

#define SR_OFFSET 0x000000010
#define SR_UIF_BIT_OFFSET 0

#define RCC_AHB1_ENABLE_REG (uint32*)0x40023830 // AHB1 bus enable register

#define GPIOA_MODER	(uint32*)0x40020000

#define GPIOA_BSRR (uint32*) 0x40020018 // bit set/reset register (atomic)
#define GPIOA_ODR (uint32*) 0x40020014
#define RCC_AHB1_GPIOA_CLOCK 0 
#define GPIOA_LED_PIN	5


int main()
{
	uint32 *TIM2temp, *TIM3temp;
	
	TIM2temp = TIM2_BASE_OFFSET;
	TIM3temp = TIM3_BASE_OFFSET;
	
	*RCC_APB1RAPB_ENABLE_REG |= (1 << RCC_APB1RAPB_TIM2EN_OFFSET);
	*RCC_APB1RAPB_ENABLE_REG |= (1 << RCC_APB1RAPB_TIM3EN_OFFSET);
	
	TIM2temp += (PSC_REG_OFFSET/4);
	*TIM2temp = TIM2_PSC;
	TIM2temp = TIM2_BASE_OFFSET;

	TIM2temp += (ARR_REG_OFFSET/4);
	*TIM2temp = ARR_TIM2;
	TIM2temp = TIM2_BASE_OFFSET;
	
	TIM3temp += (ARR_REG_OFFSET/4);
	*TIM3temp = ARR_TIM3;
	TIM3temp = TIM3_BASE_OFFSET;
	
/*	TIM2temp += DIER_REG_OFFSET;
	*TIM2temp |= (1 << DIER_UIE_BIT_OFFSET);
	TIM2temp = TIM2_BASE_OFFSET;*/
	
	TIM3temp += DIER_REG_OFFSET;
	*TIM3temp |= (1 << DIER_UIE_BIT_OFFSET);
	TIM3temp = TIM3_BASE_OFFSET;

	TIM2temp += (CR2_REG_OFFSET/4);
	*TIM2temp |= (CR2_MMS_MASTER_MODE << CR2_MMS_BIT_OFFSET);
	TIM2temp = TIM2_BASE_OFFSET;
	
	TIM3temp += (SMCR_REG_OFFSET/4);
	*TIM3temp |= (SMCR_SMS_SLAVE_MODE << SMCR_SMS_BIT_OFFSET);
	*TIM3temp |= (SMCR_TS_TI1_INPUT << SMCR_TS_BIT_OFFSET);
	TIM3temp = TIM3_BASE_OFFSET;
	
	*RCC_AHB1_ENABLE_REG |= (1 << RCC_AHB1_GPIOA_CLOCK); 
	*GPIOA_MODER &= ~(1 << (GPIOA_LED_PIN * 2 + 1));
	*GPIOA_MODER |= (1 << (GPIOA_LED_PIN * 2));
	
	*GPIOA_BSRR |= (1 << GPIOA_LED_PIN);
	
	TIM2temp += CR1_REG_OFFSET;
	*TIM2temp |= (1 << CR1_CEN_OFFSET);
	TIM2temp = TIM2_BASE_OFFSET;
	
	TIM3temp += CR1_REG_OFFSET;
	*TIM3temp |= (1 << CR1_CEN_OFFSET);
	TIM3temp = TIM3_BASE_OFFSET;
	
	TIM3temp += (SR_OFFSET/4);
	
	while(1)
	{
		if(((*TIM3temp) & (1 << SR_UIF_BIT_OFFSET)) == 1)
		{
			*TIM3temp ^= (1 << SR_UIF_BIT_OFFSET);
			*GPIOA_ODR ^= (1 << GPIOA_LED_PIN);
		}
	}
}
