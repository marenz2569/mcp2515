#ifndef SPI_H__
#define SPI_H__

#include <avr/io.h>
#include <stdint.h>

/*
 * comment out this definition to change to slave mode
 */
#define SPI_MASTER

/*
 * the spi clock is the system clock devied by 2, 4, 8, 16, 32, 64 or 128
 * change SPI_FREQ to on of these values
 */
#define SPI_FREQ	2

void spi_init(void);

uint8_t spi_wrrd(uint8_t out);

#endif
