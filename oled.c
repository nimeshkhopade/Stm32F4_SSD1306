#include <stm32f4xx.h>
int PD12, PD13, PD14, PD15, PA8, PA0, PA1, PA2;
int i = 0, count = 0,x=0, arr[1000], data=0;
uint32_t* p;
void delay(){
	int j = 0;
	for(i = 0;i < 50000; i++)
	{
		for(j = 0; j < 500; j++){}
	}
}

int main(void)
{
	/* Flash settings (see RM0090 rev9, p80) */

	FLASH->ACR =
            FLASH_ACR_LATENCY_5WS               // 6 CPU cycle wait 
          | FLASH_ACR_PRFTEN                    // enable prefetch 
          | FLASH_ACR_ICEN                      // instruction cache enable *
          | FLASH_ACR_DCEN;                     // data cache enable 
	
	
	/********* CLOCK GENERATION **********/
	
	RCC->CFGR = RCC_CFGR_PPRE2_DIV2           // APB2 prescaler 
								| RCC_CFGR_PPRE1_DIV4;          // APB1 prescaler 
				
	
	RCC->CR = RCC_CR_HSEON;         /* Enable external oscillator */

	/* Wait for locked external oscillator */
	while((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY);
	
	/* PLL config */
	RCC->PLLCFGR =
          RCC_PLLCFGR_PLLSRC_HSE                /* PLL source */
        | (4 << 0)                              /* PLL input division */
        | (168 << 6)                            /* PLL multiplication */
        | (0 << 16)                             /* PLL sys clock division */
        | (7 << 24);                            /* PLL usb clock division =48MHz */
	
	RCC->CR |= RCC_CR_PLLON;		/* Enable PLL */

	while((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);	/* Wait for locked PLL */
	
	RCC->CFGR &= ~RCC_CFGR_SW; 		/* select system clock */			/* clear */
	
	RCC->CFGR |= RCC_CFGR_SW_PLL | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2;   /* SYSCLK is PLL */
	
	while((RCC->CFGR & RCC_CFGR_SW_PLL) != RCC_CFGR_SW_PLL);		/* Wait for SYSCLK to be PLL */
	/************ /CLOCK GENERATION ************/

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; //Enable GPIO B Clock
	
	RCC->APB1ENR = 1 << 21; //I2C1 ON
	
	GPIOB->AFR[0] |= 4 << 24; //Configure PB6 to alternate function -> I2C(SCL)
	
	GPIOB->AFR[1] |= 4 << 4; //Configure PB9 to alternate function -> I2C(SDA)
	
	GPIOB->MODER |= 2 << 12 | 2 << 18; //PB6 | PB9
	
	/*************** I2C Config *******************/
	
	I2C1->CR2 |= 16 << 0; //16MHz peri clk
	
	I2C1->CCR |= 0x0050; // for 100KHz SM mode
	
	I2C1->TRISE |= 0x0011; //17 (1000ns/62.5ns = 16 + 1)
	
	I2C1->CR1 |= 1 << 0; // Enable peripheral
	
	/************ //I2C config ******************/
	
	/*
	ADC->CCR = 1 << 16; //ADCPRE is set to 4 i.e APB2 / 4 = 21MHz for ADCCLK
	
	ADC1->SMPR2 = 6 << 0 | 6 << 3 | 6 << 6; //Sampling time set to 144 cycles for channel 0
	
	ADC1->CR1 |= 0 << 24 | 1 << 8; //12bit resolution|Scan Mode
	
	ADC1->CR2 = 1 << 0 | 1 << 1 | 1 << 10; // ADON, CONT, EOCS
	
	ADC1->SQR3 = 0 << 0; //| 1 << 5 | 2 << 10 ; // put channel number(CH0,CH1,CH2)
	
	ADC1->SQR1 = 1 << 20; //L = 1 conversions (single adc conversion)
	
	ADC1->CR2 |= ADC_CR2_SWSTART; //start conversion
	*/

	while(1){
	/*
		while(ADC_SR_EOC == 0); //wait for conversion to over.
	PA0 = ADC1->DR; */
		
	} 
}
