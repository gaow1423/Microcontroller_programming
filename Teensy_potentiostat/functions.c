#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/twi.h>
#include <util/delay.h>
#include "header.h"

#define RX_BUFFER_SIZE 64
#define TX_BUFFER_SIZE 40

#define BAUD_RATE 9600
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_8MHz 	0x01
#define SCL_CLOCK 100000L


static volatile uint8_t tx_buffer[TX_BUFFER_SIZE];
static volatile uint8_t tx_buffer_head;
static volatile uint8_t tx_buffer_tail;
static volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
static volatile uint8_t rx_buffer_head;
static volatile uint8_t rx_buffer_tail;

void adc_init(){
	/* Enable SPI, Master, set clock rate fck/4*/
	SPCR = (1<<SPE)|(1<<MSTR)|(3<<SPR0);
	DDRB |= 0b11100111;
	
	//Reset chip to apply settings
	PORTF &= ~(0b00000001);
	_delay_ms(10);
	PORTF |= 0b00000001;
}

// Initialize the UART
void uart_init(uint32_t baud)
{
    DDRD |= (1<<3);//set PB3 as output
    DDRD &= ~(1<<2);//set PB2 as input
    cli();
    UBRR1 = (F_CPU / 4 / baud - 1) / 2;
    UCSR1A = (1<<U2X1);
    UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1);
    UCSR1C = (1<<UCSZ11) | (1<<UCSZ10);
    tx_buffer_head = tx_buffer_tail = 0;
    rx_buffer_head = rx_buffer_tail = 0;
    sei();
}

// Transmit a byte
void uart_putchar(uint8_t c)
{
	uint8_t i;

	i = tx_buffer_head + 1;
	if (i >= TX_BUFFER_SIZE) i = 0;
	while (tx_buffer_tail == i) ; // wait until space in buffer
	//cli();
	tx_buffer[i] = c;
	tx_buffer_head = i;
	UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1) | (1<<UDRIE1);
	//sei();
}

// Receive a byte
uint8_t uart_getchar(void)
{
        uint8_t c, i;

	while (rx_buffer_head == rx_buffer_tail) ; // wait for character
        i = rx_buffer_tail + 1;
        if (i >= RX_BUFFER_SIZE) i = 0;
        c = rx_buffer[i];
        rx_buffer_tail = i;
        return c;
}

// Return the number of bytes waiting in the receive buffer.
// Call this before uart_getchar() to check if it will need
// to wait for a byte to arrive.
uint8_t uart_available(void)
{
	uint8_t head, tail;

	head = rx_buffer_head;
	tail = rx_buffer_tail;
	if (head >= tail) return head - tail;
	return RX_BUFFER_SIZE + head - tail;
}

// Transmit Interrupt
ISR(USART1_UDRE_vect)
{
    uint8_t i;
    
    if (tx_buffer_head == tx_buffer_tail) {
        // buffer is empty, disable transmit interrupt
        UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1);
    } else {
        i = tx_buffer_tail + 1;
        if (i >= TX_BUFFER_SIZE) i = 0;
        UDR1 = tx_buffer[i];
        tx_buffer_tail = i;
    }
}

// Receive Interrupt
ISR(USART1_RX_vect)
{
    uint8_t c, i;
    
    c = UDR1;
    i = rx_buffer_head + 1;
    if (i >= RX_BUFFER_SIZE) i = 0;
    if (i != rx_buffer_tail) {
        rx_buffer[i] = c;
        rx_buffer_head = i;
    }
}

uint8_t spi_byte(uint8_t data){
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

void init_i2c() {
    PORTD &= ~(1 << 0); //Port D0 SCL
    PORTD &= ~(1 << 1); //Port D1 SDA
    TWBR = ((( F_CPU / SCL_CLOCK ) - 16) / 2);
    TWSR = 0;
    TWCR = ( 1 << TWEN ); // enable the i2c bus f
}
void start_i2c() {
    uint8_t   twstatus;
    // send START condition
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    
    // wait until transmission completed
    while ( !(TWCR & (1<<TWINT)))    ;
    twstatus = TW_STATUS & 0xF8;
    if ((twstatus != TW_START) && (twstatus != TW_REP_START)){
#ifdef DEBUG
        print("\n   |Error: Start -> ");
        phex(TWSR & 0xF8);
#endif
    }
}

void send_address_i2c(uint8_t adr, uint8_t w) {
    uint8_t   twstatus;
    TWDR = (adr<<1) | w ;
    TWCR = (1 << TWINT) | (1<<TWEN);
    
    while(!(TWCR & (1 << TWINT)));
    twstatus = TW_STATUS & 0xF8;
    if ( (twstatus != TW_MT_SLA_ACK) && (twstatus != TW_MR_SLA_ACK) ) {
#ifdef DEBUG
        print("\n   |Error: ADRESS -> ");
        phex(TWSR & 0xF8);
#endif
    }
}

void connect_i2c(uint8_t adr, uint8_t w) {
    
    start_i2c();
    send_address_i2c(adr,w);
}

void send_i2c(uint8_t data) {
    uint8_t   twstatus;
    // send data to the previously addressed device
    TWDR = data;
    TWCR = (1 << TWINT) | (1<<TWEN);
    
    // wait until transmission completed
    while(!(TWCR & (1 << TWINT)));
    twstatus = TW_STATUS & 0xF8;
    if( twstatus != TW_MT_DATA_ACK){
        
#ifdef DEBUG
        print("\n   |Error: Data -> ");
        phex(TWSR & 0xF8);
        phex(TW_MT_DATA_ACK);
#endif
    }
}

void stop_i2c() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while(TWCR & (1 << TWSTO));
}


