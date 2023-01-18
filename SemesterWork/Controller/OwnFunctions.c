#include "OwnFunctions.h"

uint32_t GLOBAL_WAIT = 0;
uint32_t GLOBAL_WAIT_R = 0;


void SetupClocks(){
	//	Enable co-processors | not really nesseceary in this project, but I let that as is.
		SCB->CPACR |= 3UL << 20;
		SCB->CPACR |= 3UL << 22;

	//	Set flash memory wait states to match core clocks 2ws for 60 < HCLK <= 84(which is maximum clock for our core) and play around with new sweeties
		FLASH->ACR |= FLASH_ACR_LATENCY_2WS | \
				FLASH_ACR_DCEN | \
				FLASH_ACR_ICEN | \
				FLASH_ACR_PRFTEN;

	//	That sets SYSCLK divider
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | 				//AHB  prescaler = 1
		    		RCC_CFGR_PPRE1_DIV2 | 				//APB1 prescaler = 2
					RCC_CFGR_PPRE2_DIV1; 				//APB2 prescaler = 1

	//	Enable external clock source, at blackPill that is 25MHz
		RCC->CR |=  1 << 16;

	//	Wait until external clock is ready
		while (!(RCC->CR & RCC_CR_HSERDY));

	//	Set zero bits for PLL prescalers (// modify only nedded field)
		RCC->PLLCFGR &= 0xFFBC8000;

	//	Sets the M N P scalers for PLL
		RCC->PLLCFGR |= 25 | (336 << 6 ) | (1 << 16);

	// Set External source for PLL
		RCC->PLLCFGR |= 1 << 22;

	//	Set Q scaler for PLL
		RCC->PLLCFGR |= 7 << 24;

	// Enable PLL
		RCC->CR |= 1 << 24;
		while(!(RCC->CR & RCC_CR_PLLRDY));

	//	Set PLL as selected system clock
		RCC->CFGR |= 2;
		while((RCC->CFGR & RCC_CFGR_SW_1) != RCC_CFGR_SW_1);
}

void SetupGPIO() {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;
	RCC->AHB1ENR |= 133;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	GPIOA->MODER |= (2<<10)|(2<<12)|(2<<14)|(0b01010101 << 16);  // Alternate functions for PA5, PA6, PA7 and Output for PA9
	GPIOA->OSPEEDR |= (3<<10)|(3<<12)|(3<<14)|(3<<18)|(0xFF << 16);  // VERY HIGH Speed for PA5, PA6, PA7, PA9
//	GPIOA->OSPEEDR = 201391104;

	GPIOC->MODER |= (0b01 << 26);
	GPIOC->OSPEEDR |= (3<<26);

	GPIOA->AFR[0] |= (5<<20)|(5<<24)|(5<<28);   // AF5(SPI1) for PA5, PA6, PA7
//	GPIOA->AFR[0] = 1431306240;
	GPIOC->ODR |= (1<<13); //На блэк пилл это почему-то выключает C13
}

void SetupSPI() {
  RCC->APB2ENR |= (1<<12);  // Enable SPI1 CLock

  SPI1->CR1 &= ~(0x3);   // CPOL=0, CPHA=0

  SPI1->CR1 &= ~(1<<2);  // Slave Mode

//  SPI1->CR1 |= (3<<3);  // BR[2:0] = 011: fPCLK/16, PCLK2 = 80MHz, SPI clk = 5MHz
//  SPI1->CR1 |= (3<<3);

  SPI1->CR1 &= ~(1<<7);  // LSBFIRST = 0, MSB first

  SPI1->CR1 |= (1<<8) | (1<<9);  // SSM=1, SSi=1 -> Software Slave Management

  SPI1->CR1 &= ~(1<<10);  // RXONLY = 0, full-duplex

  SPI1->CR1 &= ~(1<<11);  // DFF=0, 8 bit data

  SPI1->CR2 = 0;

  SPI1->CR1 = 576;
  SPI1->CR2 = 0;
  SPI1->SR = 2;
  SPI1->CRCPR = 7;
  SPI1->RXCRCR = 0;
  SPI1->TXCRCR = 0;
  SPI1->I2SCFGR = 0;
  SPI1->I2SPR = 0;
}

void usleep(uint32_t amount){
	for(uint32_t i = 0; i < amount*millisecond; i++);
}

void SPI_Transmit (uint8_t *data, int size) {
	int i=0;
	while (i<size)
	{
	   while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	   SPI1->DR = data[i];  // load the data into the Data Register
	   i++;
	}

	while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
	uint8_t temp = SPI1->DR;
	temp = SPI1->SR;

}

void SPI_Receive (uint8_t *data, int size) {
	while (size) {
		while (!((SPI1->SR) &(1<<0))){};
	  *data++ = (SPI1->DR);
		size--;
	}
}

void USART2_IRQHandler(void){
	char lol = USART2->DR;
	USART2->DR = lol;
}

void SetupTimer(uint16_t milliseconds, uint32_t await){
	GLOBAL_WAIT_R = await;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 41999;
	TIM2->ARR = 1000*2;
//	TIM2->EGR = TIM_EGR_UG;
	TIM2->DIER = TIM_DIER_UIE;
}

void TIM2_IRQHandler() {
  if(TIM2->SR & TIM_SR_UIF) {
    TIM2->SR &= ~TIM_SR_UIF; // clear interrupt of the timer
  }
  if (GLOBAL_WAIT >= GLOBAL_WAIT_R){
	  GPIOC->ODR &= ~(1<<13);
	  GPIOA->ODR &= ~(0xFF<<8);
  }
  ++GLOBAL_WAIT;
}

void clearTimer(){

	GLOBAL_WAIT = 0;
	GPIOC->ODR |= (1<<13);
}












