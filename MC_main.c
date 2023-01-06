#include <stdint.h>
#include <stm32f401xc.h>

#define MySYSCLK 84000000
#define MyAHBCLK (MySYSCLK/1)
#define MyAPB1CLK (MySYSCLK/2)
#define MyAPB2CLK (MySYSCLK/1)
#define BOUNDRATE 19200


void USART2_IRQHandler(void);
void InitClocks();
void GPIOConfig();
void SPIConfig();
void SPI_Transmit (uint8_t *data, int size);
void SPI_Receive (uint8_t *data, int size);

int main(void) {
/**
 * AF07 for PB6 - TX and for PB7 - RX (PB on GPIO_B) but that is USART1, we need USART2 just ff
 * so we are going to use A2(TX) and A3(RX) with AF 7
*/
	InitClocks();
	GPIOConfig();
	SPIConfig();

	uint8_t buf[8] = {0};



	/* Loop forever */
	for(;;){
		SPI_Receive(&buf, 8);
		if(buf[0] != 0) {
				  int k = 0;
		}
//		usleep(100000000);
	}
}

void USART2_IRQHandler(void){
	char lol = USART2->DR;
	USART2->DR = lol;
}

void InitClocks(){
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

void GPIOConfig() {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= 133;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
//	RCC->AHB1ENR |= (1<<0);  // Enable GPIO Clock
	GPIOA->MODER |= (2<<10)|(2<<12)|(2<<14);  // Alternate functions for PA5, PA6, PA7 and Output for PA9
	GPIOA->OSPEEDR |= (3<<10)|(3<<12)|(3<<14)|(3<<18);  // HIGH Speed for PA5, PA6, PA7, PA9
	GPIOA->OSPEEDR = 201391104;

	GPIOA->AFR[0] |= (5<<20)|(5<<24)|(5<<28);   // AF5(SPI1) for PA5, PA6, PA7
	GPIOA->AFR[0] = 1431306240;
	GPIOC->IDR = 57344;
}

void SPIConfig() {
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

void SPI_Transmit (uint8_t *data, int size)
{

	/************** STEPS TO FOLLOW *****************
	1. Wait for the TXE bit to set in the Status Register
	2. Write the data to the Data Register
	3. After the data has been transmitted, wait for the BSY bit to reset in Status Register
	4. Clear the Overrun flag by reading DR and SR
	************************************************/

	int i=0;
	while (i<size)
	{
	   while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	   SPI1->DR = data[i];  // load the data into the Data Register
	   i++;
	}


/*During discontinuous communications, there is a 2 APB clock period delay between the
write operation to the SPI_DR register and BSY bit setting. As a consequence it is
mandatory to wait first until TXE is set and then until BSY is cleared after writing the last
data.
*/
	while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication

	//  Clear the Overrun flag by reading DR and SR
	uint8_t temp = SPI1->DR;
	temp = SPI1->SR;

}

void SPI_Receive (uint8_t *data, int size) {
	/************** STEPS TO FOLLOW *****************
	1. Wait for the BSY bit to reset in Status Register
	2. Send some Dummy data before reading the DATA
	3. Wait for the RXNE bit to Set in the status Register
	4. Read data from Data Register
	************************************************/

	while (size) {
//		while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
//		SPI1->DR = 0;  // send dummy data
		while (!((SPI1->SR) &(1<<0))){};  // Wait for RXNE to set -> This will indicate that the Rx buffer is not empty
	  *data++ = (SPI1->DR);
		size--;
	}
}

