#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "spi.h"
#include "mcp2515.h"
#include "mcp2515_defs.h"
#include "mcp2515_config.h"

void mcp2515_performpgm(const uint8_t *cmds, uint8_t len)
{
	const uint8_t * PROGMEM end = cmds + len;
	uint8_t sreg = SREG;

	cli();
	MCP2515_enable;

	while (cmds < end) {
		spi_wrrd(pgm_read_byte(cmds++));
	}

	MCP2515_disable;
	SREG = sreg;
}

void mcp2515_init(void)
{
#ifdef MCP2515_OUTPUT
	mcp2515_io_pins.output = 0;
#endif
#ifdef MCP2515_INPUT
	mcp2515_io_pins.input = 0;
#endif

	if (!(SPCR & _BV(SPE)))
		spi_init();

	MCP2515_CS_DDR |= _BV(MCP2515_CS_PIN);
	MCP2515_disable;

	/* reset */
	mcp2515_perform(MCP2515_RESET);

	_delay_ms(5);

	/* configuration mode */
	/* cnf3, cnf2, cnf1, caninte */
	mcp2515_perform(MCP2515_WRITE, MCP2515_CNF3,
	        0x05,
	        0xf1,
	        0x41,
	        0x03);

	/* rxb0ctrl */
	mcp2515_perform(MCP2515_WRITE, MCP2515_RXB0CTRL,
	        0x60);
	/* rxb1ctrl */
	mcp2515_perform(MCP2515_WRITE, MCP2515_RXB1CTRL,
	        0x60);

	/* normal mode */
	mcp2515_perform(MCP2515_WRITE, MCP2515_CANCTRL,
	        0x00);
}

#ifdef MCP2515_OUTPUT
void mcp2515_output(void)
{
	uint8_t sreg = SREG;

	cli();
	MCP2515_enable;

	spi_wrrd(MCP2515_WRITE);
	spi_wrrd(MCP2515_BFPCTRL);
	spi_wrrd((mcp2515_io_pins.output << 4) | MCP2515_BFPCTRL_B0BFE | MCP2515_BFPCTRL_B1BFE);

	MCP2515_disable;
	SREG = sreg;
}
#endif

#ifdef MCP2515_INPUT
void mcp2515_input(void)
{
	uint8_t sreg = SREG;

	cli();
	MCP2515_enable;

	spi_wrrd(MCP2515_READ);
	spi_wrrd(MCP2515_TXRTSCTRL);
	mcp2515_io_pins.input = spi_wrrd(0) >> 3;

	MCP2515_disable;
	SREG = sreg;
}
#endif

void can_rxh(uint8_t buffer)
{
	uint8_t c = 0,
	        sreg = SREG;

	cli();
	MCP2515_enable;

	spi_wrrd(MCP2515_READ_RXBUF + 0x04 * buffer);
	for (c=0; c<4; c++) {
		can_frame.addr[c] = spi_wrrd(0);
	}
	can_frame.dlc = spi_wrrd(0);
	for (c=0; c<can_get_len; c++) {
		can_frame.data[c] = spi_wrrd(0);
	}

	MCP2515_disable;
	SREG = sreg;
}

void can_send(uint32_t addr, uint8_t len, const uint8_t *data)
{
	uint8_t sreg = SREG;

	cli();
	MCP2515_enable;

	spi_wrrd(MCP2515_LOAD_TXBUF);
	spi_wrrd((addr >> 24) & 0xff);
	spi_wrrd((addr >> 16) & 0xff);
	spi_wrrd((addr >> 8) & 0xff);
	spi_wrrd(addr & 0xff);
	spi_wrrd(len);
	len &= 0x0f;
	while (len--) {
		spi_wrrd(*data++);
	}

	MCP2515_disable;
	SREG = sreg;

	mcp2515_perform(MCP2515_RTS | 0x01);
}

uint8_t can_tx_busy(void)
{
	uint8_t status,
	        sreg = SREG;

	cli();

	spi_wrrd(MCP2515_READ_STATUS);
	status = spi_wrrd(0);

	SREG = sreg;

	return status & 0x04;
}
