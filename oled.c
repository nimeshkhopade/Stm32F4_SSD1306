#include <stm32f4xx.h>
int PD12, PD13, PD14, PD15, PA8, PA0, PA1, PA2;
int i = 0, count = 0,x=0, arr[1000], data=0;
uint32_t* p;
void delay(){
	int j = 0;
	for(i = 0;i < 1; i++)
	{
		for(j = 0; j < 1; j++){}
	}
}

void i2c_start() {
	I2C1->CR1 |= 1 << 8; //Start I2C
	while(!(I2C1->SR1&0x0001));
}

void i2c_stop() {
	I2C1->CR1 |= 1 << 9; //Stop I2C
	while(I2C1->SR2&0x0002);
}

void i2c_write(int c) {
	I2C1->DR = c; //send data
	delay();
	while(!(I2C1->SR1 & (1 << 7)));
}

void i2c_addr(unsigned int i) {
	char res;
	I2C1->DR = i | 0; //send addr
	delay();
	while(!(I2C1->SR1&0x0002));
	res = I2C1->SR2;
}

void oled_cmd(int cmd) {
	i2c_start();
	i2c_addr(0x3C<<1);
	i2c_write(0x00);
	i2c_write(cmd);
	i2c_stop();
}

void oled_clear() {
	int i = 0;
	
	oled_cmd(0x21);
	oled_cmd(0x00);
	oled_cmd(127);
	oled_cmd(0x22);
	oled_cmd(0x00);
	oled_cmd(0x07);
	
	i2c_start();
	i2c_addr(0x3C << 1);
	i2c_write(0x40);
	
	for(i = 0; i<1024; i++) {
		i2c_write(0);
	}
	
	oled_cmd(0x21);
	oled_cmd(0x00);
	oled_cmd(127);
	oled_cmd(0x22);
	oled_cmd(0x00);
	oled_cmd(0x07);
	/*
	i2c_start();
	i2c_addr(0x3C << 1);
	i2c_write(0x40);
	*/
}

void oled_init() {
	oled_cmd(0xAE);
	oled_cmd(0xD5);
	oled_cmd(0x80);
	oled_cmd(0xA8);
	oled_cmd(0x3F);
	oled_cmd(0xD3);
	oled_cmd(0x00);
	oled_cmd(0x40 | 0x00);
	oled_cmd(0x8D);
	oled_cmd(0x14);
	oled_cmd(0x20);
	oled_cmd(0x00);
	oled_cmd(0xA0 | 0x1);
	oled_cmd(0xC8);
	oled_cmd(0xDA);
	oled_cmd(0x12);
	oled_cmd(0x81);
	oled_cmd(0xCF);
	oled_cmd(0xD9);
	oled_cmd(0xF1);
	oled_cmd(0xDB);
	oled_cmd(0x80);
	oled_cmd(0xA4);
	oled_cmd(0xA6);
	oled_cmd(0xAF);
	oled_clear();
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
	
	GPIOB->OTYPER = 1 << 6 | 1 << 9; // Open Drain
	
	GPIOB->OSPEEDR |= 1 << 12 | 1 << 18; // Medium Speed
	
	GPIOB->PUPDR |= 1 << 12 | 1 << 18; // PULLUP VERY IMPORTANT
	
	/*************** I2C Config *******************/
	
	I2C1->CR2 |= 16 << 0; //16MHz peri clk
	
	I2C1->CCR |= 0x0050; // for 100KHz SM mode
	
	I2C1->TRISE = 0x0011; //17 (1000ns/62.5ns = 16 + 1)
	
	I2C1->CR1 |= 1 << 0; // Enable peripheral
	
	/************ //I2C config ******************/
	
	oled_init();
	i2c_start();
	i2c_addr(0x3C<<1);
	i2c_write(0x40);
	i2c_write(0xFF);
	i2c_write(0xFF);
	i2c_stop();
	
	while(1){
	
	} 
}
