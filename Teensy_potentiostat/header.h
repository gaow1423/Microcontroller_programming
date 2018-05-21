

void uart_init(uint32_t baud);
void uart_putchar(uint8_t c);
uint8_t uart_getchar(void);
uint8_t uart_available(void);

//SPI
uint8_t spi_byte(uint8_t data);

//I2C COMMUNICATION
void init_i2c();
void start_i2c();
void send_address_i2c(uint8_t adr, uint8_t w);
void connect_i2c(uint8_t,uint8_t);
void send_i2c(uint8_t);
void stop_i2c();

//#endif
