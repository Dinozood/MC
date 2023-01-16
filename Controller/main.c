#include "OwnFunctions.h"
//			GPIOA->ODR ^= (2 << 8); // for right 10 is backward, so 01 is forward;
//			GPIOA->ODR ^= (2 << 10); // same for left
int main(void) {

	SetupClocks();
	SetupGPIO();
	SetupSPI();
	SetupTimer(1000, 30);

	NVIC_EnableIRQ(TIM2_IRQn);
	uint8_t buf[BUFFER_SIZE] = {0};
    int kik = 0;
    TIM2->CR1 |= 1;										//
    /* Loop forever */

	for(;;){
		SPI_Receive(&buf, BUFFER_SIZE);
		GPIOA->ODR = buf[0];
		GPIOA->ODR = buf[1];
		clearTimer();
	}
}

