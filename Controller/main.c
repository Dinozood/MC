#include "OwnFunctions.h"
//			GPIOA->ODR ^= (2 << 8); // for right 10 is backward, so 01 is forward;
//			GPIOA->ODR ^= (2 << 10); // same for left
int main(void) {

	SetupClocks();
	SetupGPIO();
	SetupSPI();
	SetupTimer(1000, 2);

	NVIC_EnableIRQ(TIM2_IRQn);
	uint8_t buf[BUFFER_SIZE] = {0};
	uint32_t cmd = 0;
    TIM2->CR1 |= 1;
    /* Loop forever */
	for(;;){
		SPI_Receive(&buf, BUFFER_SIZE);
		cmd = ( ( (buf[1]&0x3) << 2) | (buf[0]&0x3) ) << 8;
		GPIOA->ODR = cmd;
		clearTimer();
	}
}

