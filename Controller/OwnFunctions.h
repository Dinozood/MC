#ifndef OWNFUNCTIONS_H_
#define OWNFUNCTIONS_H_

#include <stdint.h>
#include <stm32f401xc.h>

#define MySYSCLK 84000000
#define MyAHBCLK (MySYSCLK/1)
#define MyAPB1CLK (MySYSCLK/2)
#define MyAPB2CLK (MySYSCLK/1)
#define BOUNDRATE 19200
#define millisecond (84000000/1000/16)
#define BUFFER_SIZE 2

void SetupClocks();
void SetupGPIO();
void SetupSPI();
void usleep(uint32_t amount);
void SPI_Transmit (uint8_t *data, int size);
void SPI_Receive (uint8_t *data, int size);
void SetupTimer(uint16_t milliseconds, uint32_t await);
void clearTimer();

#endif /* OWNFUNCTIONS_H_ */
