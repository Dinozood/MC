#include <stdint.h>
#include "stm32f401xc.h"

uint32_t GLOBAL_WAIT = 0;
uint32_t GLOBAL_WAIT_R = 0;

void SetupClocks() {
	//	Set flash memory wait states to match core clocks 0ws for 0 < HCLK <= 16
		FLASH->ACR = FLASH_ACR_LATENCY_0WS | \
				FLASH_ACR_DCEN | \
				FLASH_ACR_ICEN | \
				FLASH_ACR_PRFTEN;

	//	That sets SYSCLK divider
		RCC->CFGR = RCC_CFGR_HPRE_DIV2 | 				//AHB  prescaler = 2
					RCC_CFGR_PPRE1_DIV2| 				//APB1 prescaler = 16
					RCC_CFGR_PPRE2_DIV2; 				//APB2 prescaler = 16

	//	Enable external clock source, at blackPill that is 25MHz
		RCC->CR =  1 << 16;

	//	Wait until external clock is ready
		while (!(RCC->CR & RCC_CR_HSERDY));

	//	Set zero bits for PLL prescalers (// modify only nedded field)
		RCC->PLLCFGR &= 0xFFBC8000;

	//	Sets the M N P Q scalers for PLL
		RCC->PLLCFGR |= 25 | (192 << 6 ) | (1 << 16) | (4 << 24);

	// Set External source for PLL
		RCC->PLLCFGR |= 1 << 22;

	//	Set Q scaler for PLL
//		RCC->PLLCFGR |= 4 << 24;

	// Enable PLL
		RCC->CR |= 1 << 24;
		while(!(RCC->CR & RCC_CR_PLLRDY));

	//	Set PLL as selected system clock
		RCC->CFGR |= 2;
		while((RCC->CFGR & RCC_CFGR_SW_1) != RCC_CFGR_SW_1);
}

void SetupGPIO() {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	GPIOC->MODER |= (0b01 << 26);
	GPIOC->OSPEEDR |= (3<<26);
	GPIOC->ODR |= (1<<13); //На блэк пилл это почему-то выключает C13
}

void SetupTimer(uint16_t milliseconds) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 24000-1;
	TIM2->ARR = milliseconds;
	TIM2->DIER = TIM_DIER_UIE;
}

void TIM2_IRQHandler() {
	if(TIM2->SR & TIM_SR_UIF) {
		TIM2->SR &= ~TIM_SR_UIF; // clear interrupt of the timer
  	}
	GPIOC->ODR ^= (1 << 13);
}

int main(void) {

	SetupClocks();
	SetupGPIO();
	SetupTimer(1000);

	NVIC_EnableIRQ(TIM2_IRQn);
    TIM2->CR1 |= 1;
    for(;;){
		//Do some work
	}
}
