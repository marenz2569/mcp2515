#include "spi.h"

void spi_init(void)
{
	DDRB |=
#ifdef SPI_MASTER
		_BV(DDB3) | _BV(DDB5)
#else
		_BV(DDB4)
#endif
	;

	SPCR |= _BV(SPE)
#ifdef SPI_MASTER
	     | _BV(MSTR)
#if SPI_FREQ == 32 || SPI_FREQ == 64 || SPI_FREQ == 128
	     | _BV(SPR1)
#endif
#if SPI_FREQ == 8 || SPI_FREQ == 16 || SPI_FREQ == 32 || SPI_FREQ == 64
	     | _BV(SPR0)
#endif
#if SPI_FREQ != 2 && SPI_FREQ != 4 && SPI_FREQ != 8 && SPI_FREQ != 16 && SPI_FREQ != 32 && SPI_FREQ != 64 && SPI_FREQ != 128
#error "(SPI) Select a valid prescaler!"
#endif
#endif
	;

#ifdef SPI_MASTER
#if SPI_FREQ == 2 || SPI_FREQ == 8 || SPI_FREQ == 32 || SPI_FREQ == 64
	SPSR |= _BV(SPI2X);
#endif
#endif
}

uint8_t spi_wrrd(uint8_t out)
{
	SPDR = out;
	while (!(SPSR & _BV(SPIF)))
		;

	return SPDR;
}
