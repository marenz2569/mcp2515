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

void nothing(void)
{

}

ISR(INT0_vect)
{
	can_rx_handler(&nothing);
}
