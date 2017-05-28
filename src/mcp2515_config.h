#ifndef MCP2515_CONFIG_H__
#define MCP2515_CONFIG_H__

#define MCP2515_CS_DDR	DDRB
#define MCP2515_CS_PORT	PORTB
#define MCP2515_CS_PIN	PORTB2

/**
 * undef to disable output via RX(0|1)BF pins
 * saves 1 byte of ram and 4 bytes of flash
 */
//#define MCP2515_OUTPUT

/**
 * undef to disable input via TX(0|1|2)RTS pins
 * saves 1 byte of ram and 4 bytes of flash
 */
//#define MCP2515_INPUT

#endif
