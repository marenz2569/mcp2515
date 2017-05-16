#ifndef MCP2515_H__
#define MCP2515_H__

#include <avr/pgmspace.h>

#include "mcp2515_config.h"
#include "mcp2515_defs.h"

#define MCP2515_enable	MCP2515_CS_PORT &= ~_BV(MCP2515_CS_PIN)
#define MCP2515_disable	MCP2515_CS_PORT |= _BV(MCP2515_CS_PIN)

/**
 * perform spi operation on mcp2515
 * @param ... spi commands seperated by komma
 */
#define mcp2515_perform(...) \
	do { \
		static const uint8_t _mycmds[] PROGMEM = { __VA_ARGS__ }; \
		mcp2515_performpgm(_mycmds, sizeof(_mycmds)); \
	} while (0)

/**
 * get uint32_t from can_frame.addr in right order and chop off unwanted bits
 */
#define can_addr \
	(((uint32_t) ((uint32_t) can_frame.addr[0] << 24) | ((uint32_t) can_frame.addr[1] << 16) | ((uint32_t) can_frame.addr[2] << 8) | (uint32_t) can_frame.addr[3]) & (MCP2515_RX_EID_FLAG | MCP2515_RX_ID_MASK | (can_is_extended?(MCP2515_RX_EID_MASK):0)))

/**
 * get standard id of received can frame
 * @see can_rxh()
 */
#define can_get_std_id \
	((can_addr & MCP2515_RX_ID_MASK) >> 21)

/**
 * get extended id of received can frame
 * @see can_rxh()
 */
#define can_get_ex_id \
	(can_addr & MCP2515_RX_EID_MASK)

/**
 * check if received can frame is with extended id
 * @see can_rxh()
 */
#define can_is_extended \
	(can_frame.addr[1] & 0x08)
  
/**
 * check if received can frame is without extended id
 * @see can_rxh()
 */
#define can_is_standard \
	(!can_is_extended)

/**
 * check if received can frame has rtr set
 * @see can_rxh()
 */
#define can_is_remote_frame \
	(can_is_extended?(can_frame.dlc & 0x40):(can_frame.data[1] & 0x10))

/**
 * get length of received can frame
 * @see can_rxh()
 */
#define can_get_len \
	(can_frame.dlc & 0x0f)

/**
 * modify 11 bit can standard id for mcp2515
 * @see can_send
 * @see can_send_rtr
 */
#define can_std_id(id) \
	(((uint32_t) (id) << 21) & 0xffe00000)

/**
 * modify 18 bit can extended id for mcp2515
 * @see can_send
 * @see can_send_rtr
 */
#define can_ex_id(id) \
	(((uint32_t) (id) & 0x0003ffff) | 0x00080000)

/**
 * send a can frame with rtr
 * @see can_send()
 */
#define can_send_rtr(addr, len, data) \
	can_send(addr, (len) | 0x40, data)

#ifdef MCP2515_INPUT
#define MCP2515_INPUT_IO(x) \
	((mcp2515_io_pins.input & _BV(x))?1:0)

/**
 * get state of mcp2515 io pin 0
 * @see mcp2515_input()
 */
#define mcp2515_input_0 \
	MCP2515_INPUT_IO(0)

/**
 * get state of mcp2515 io pin 1
 * @see mcp2515_input()
 */
#define mcp2515_input_1 \
	MCP2515_INPUT_IO(1)

/**
 * get state of mcp2515 io pin 2
 * @see mcp2515_input()
 */
#define mcp2515_input_2 \
	MCP2515_INPUT_IO(2)
#endif

void mcp2515_performpgm(const uint8_t *cmds, uint8_t len);

/**
 * init mcp2515
 */
void mcp2515_init(void);

/**
 * used to save state of io pins from mcp2515
 * @see mcp2515_output()
 * @see mcp2515_input()
 */
struct {
#ifdef MCP2515_OUTPUT
	/**
	 * used to store state of mcp2515 output pins
	 * @note 2 pins available for output
	 * @note last 2 bits are used for pin RX0BF and RX1BF
	 * @see mcp2515_output()
	 */
	uint8_t output;
#endif
#ifdef MCP2515_INPUT	
	/**
	 * used to store state of mcp2515 input pins
	 * @note 3 pin available for input
	 * @see mcp2515_input()
	 */
	volatile uint8_t input;
#endif
} mcp2515_io_pins;

#ifdef MCP2515_OUTPUT
/**
 * used to write state form mcp2515_io_pins.output to output pins of mcp2515
 * @see mcp2515_io_pins
 */
void mcp2515_output(void);
#endif

#ifdef MCP2515_INPUT
/**
 * used to write state of input pins from mcp2515 to mcp2515_io_pins.input
 * @see mcp2515_io_pins
 */
void mcp2515_input(void);
#endif

/**
 * used to save a receive buffer
 */
struct {
	uint8_t addr[4],
	        dlc,
	        data[8];
} can_frame;

/**
 * write a message from receive buffer to can_frame struct
 * @see can_frame
 * @param buffer either 0 or 1 depending on the receive buffer you want to get
 * @see can_get_std_id
 * @see can_get_ex_id
 * @see can_is_extended
 * @see can_is_standard
 * @see can_is_remote_frame
 * @see can_get_len
 */
void can_rxh(uint8_t buffer);

/**
 * send a can data frame
 * @param addr can id and extended id
 * @note to be used with macro can_std_id(id) and can_ex_id(id)
 * @param len length of data
 * @param data pointer to data
 */
void can_send(uint32_t addr, uint8_t len, const uint8_t *data);

/**
 * check if controller is busy transmitting
 */
uint8_t can_tx_busy(void);

/**
 * function that executes rxhandler function when can controller has got a packet
 * @param rxhandler function that gets executed
 */
void can_rx_handler(void (*rx_handler) (void));

#endif
