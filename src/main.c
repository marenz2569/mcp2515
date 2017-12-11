#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <string.h>

#include "mcp2515.h"
#include "mcp2515_defs.h"
#include "spi.h"

void something(void)
{

}

int main(void)
{
	uint8_t i;

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

		/* read the received packets */
		for (i=0; i<2; i++) {
			if (can_buffer[i].state == FILLED) {
				memcpy(&can_frame, can_buffer + i, sizeof(can_frame));
				/* do something with the received messages here */
				something();
				/* free the buffer */
				can_buffer[i].state = FREE;
			}
		}
	}

	return 0;
}

ISR(INT0_vect)
{
	can_rx_handler();
}
