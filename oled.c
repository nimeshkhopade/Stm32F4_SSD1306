#include <stm32f4xx.h>
long PD12, PD13, PD14, PD15;
void delay(){
	int j = 0;
	for(int i = 0;i < 50000; i++)
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
	
	/* Configure clocks
	 * Max SYSCLK: 168MHz
	 * Max AHB:  SYSCLK
	 * Max APB1: SYSCLK/4 = 48MHz
	 * Max APB2: SYSCLK/2 = 86MHz
	 * + enable sys clock output 2 with clock divider = 4 
	*/
	/*
	RCC->CFGR =
          0x0                           // Clock output 2 is SYSCLK (RCC_CFGR_MCO2) 
        | ( 0x6 << 27)                  // Clock output divider 
        | RCC_CFGR_PPRE2_DIV2           // APB2 prescaler 
        | RCC_CFGR_PPRE1_DIV4;          // APB2 prescaler 
				*/
	
	
	/* Clock control register */
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

	/* crystal:  8MHz
	 * PLL in:   2MHz (div 4)
	 * PLL loop: 336MHz (mul 168)
	 * PLL out:  168MHz (div 2)
	 * PLL usb:  48MHz (div 7)
	*/
	
	RCC->CR |= RCC_CR_PLLON;		/* Enable PLL */

	while((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);	/* Wait for locked PLL */
	
	RCC->CFGR &= ~RCC_CFGR_SW; 		/* select system clock */			/* clear */
	
	RCC->CFGR |= RCC_CFGR_SW_PLL | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2;   /* SYSCLK is PLL */
	
	while((RCC->CFGR & RCC_CFGR_SW_PLL) != RCC_CFGR_SW_PLL);		/* Wait for SYSCLK to be PLL */

	RCC->AHB1ENR |= 1 << 3;
	
	GPIOD->MODER   = GPIO_MODER_MODE15_0|GPIO_MODER_MODE14_0|GPIO_MODER_MODE13_0|GPIO_MODER_MODE12_0;		//0x55 << 16; /* output */
	
	int x =12;
	
	while(1){
	delay();
	GPIOD->ODR ^= 0xf000;
	delay();
	GPIOD->ODR ^= GPIOD->ODR;
	delay();
	for(int k = 0; k < 4; k++){
		delay();
		GPIOD->ODR ^= 1 << x;
		PD12 = GPIOD->ODR & 0x00001000;
		PD13 = GPIOD->ODR & 0x00002000;
		PD14 = GPIOD->ODR & 0x00004000;
		PD15 = GPIOD->ODR & 0x00008000;
		delay();
		x++;
	}
	GPIOD->ODR ^= GPIOD->ODR;
	for(int k = 0; k < 4; k++){
		x--;
		delay();
		GPIOD->ODR ^= 1 << x;
		PD12 = GPIOD->ODR & 0x00001000;
		PD13 = GPIOD->ODR & 0x00002000;
		PD14 = GPIOD->ODR & 0x00004000;
		PD15 = GPIOD->ODR & 0x00008000;
		delay();
	}
	GPIOD->ODR ^= GPIOD->ODR;
} 
}
