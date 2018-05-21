#include <util/twi.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "header.h"
#include <util/delay.h>
#include <string.h>

#define BAUD_RATE 9600
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_8MHz 	0x01
#define I2C_READ 1
#define I2C_WRITE 0

// write a string to the uart
//#define uart_print(s) uart_print_P(PSTR(s))

void I2C_Call(uint8_t a){
    //uint_8 a;
	init_i2c();
	connect_i2c(72, 0);//68
	send_i2c(1);//59
	send_i2c(0);
	connect_i2c(72, 0);//68
	send_i2c(16);//59
	send_i2c(7);
	connect_i2c(72, 0);//68
	send_i2c(17);//59
	send_i2c(a);
	connect_i2c(72, 0);//68
	send_i2c(18);//59
	send_i2c(3);
	stop_i2c();

}

#define uart_print(s) uart_print_P(PSTR(s))
void uart_print_P(char const *str)
{
	char c;
	int k = 0;
	while (1) {
		c = pgm_read_byte(str++);
		//c = str[k++];
		if (!c) break;
		uart_putchar(c);
	}
}

void print(char *str)
{
	int count = 0;
	while(str[count] != '\0')
	{
		uart_putchar(str[count]);
		count++;
	}
}

double Calibrate(double m)
{
    double actual_value = 0.9921861*m + 3.116;
    return actual_value;
}


void double_char(char buff[], double x, int n)
{
    
    int i = (int)x;//integer part of double
    x = x - i;//reminder part of double
    int s = (int)(floor (log10 (abs (i))) + 1);//the number of digits in integer part of double
    //printf("%d\n", s);
    int r;
    int k;
    for (k = 0; k < s; k++)
    {
        r = i/pow(10, s-k-1);
        buff[k] = r + 48;
        i = i - (r * pow(10, s-k-1));
    }
    
    buff[s] = '.';
    int j;
    for (j = 0; j < n; j++)
    {
        x = x * 10;
        r = (int)x;
        x = x - r;
        buff[s+j+1] = r + 48;
    }
}

int main(void)
{

	uint8_t data1;
	uint16_t value;
    //double out;
    
	char temp_str[128];
	//char rec[64];
	//    	char a;
	//CPU_PRESCALE(CPU_8MHz); //3.3volt compatible
	DDRB = 0b11111111; //Port B all output
	DDRD = 0b01000000; //LED is set as an output
	DDRF = 0b00000011; //Set F0 and F1 as ouputs(for RST and CS respectively)
    adc_init();
    uint8_t c;
    CPU_PRESCALE(0);
	uart_init(BAUD_RATE);
    //uart_print("UART Example\r\n");
    //char rec[32];
    char buf[32];
    
    char buf_i[32];
    char buf_i_n[32];
    char buf_v[32];
    char buf_avg[32];
    //sprintf(buf, "im wei gao\n");
    int count = 0;
    //I2C_Call();
    double v_out;
    double i_out;
    double i_out_n;
    double i_avg;
    
    double sam[4];
    sam[0] = 0.0;
    sam[1] = 0.0;
    sam[2] = 0.0;
    sam[3] = 0.0;
    //int ind = 1;
    int avg_count = 0;
    
    int count_send = 0;
    
    double cali;
    
	while(1)
	{
        count_send++;
        

		if (uart_available()) {
			c = uart_getchar();
			buf[count] = c;
            count++;
            
		}
		else{
            
            if (strcmp(buf, "0") == 0 || strcmp(buf, "0.00V") == 0)
            {
                I2C_Call(160);
            }
            else if (strcmp(buf, "-1") == 0 || strcmp(buf, "-0.025V") == 0)
            {
                I2C_Call(161);
            }
            else if (strcmp(buf, "-2") == 0 || strcmp(buf, "-0.05V") == 0)
            {
                I2C_Call(162);
            }
            else if (strcmp(buf, "-4") == 0 || strcmp(buf, "-0.10V") == 0)
            {
                I2C_Call(163);
            }
            else if (strcmp(buf, "-6") == 0 || strcmp(buf, "-0.15V") == 0)
            {
                I2C_Call(164);
            }
            else if (strcmp(buf, "-8") == 0 || strcmp(buf, "-0.20V") == 0)
            {
                I2C_Call(165);
            }
            else if (strcmp(buf, "-10") == 0 || strcmp(buf, "-0.25V") == 0)
            {
                I2C_Call(166);
            }
            else if (strcmp(buf, "-12") == 0 || strcmp(buf, "-0.30V") == 0)
            {
                I2C_Call(167);
            }
            else if (strcmp(buf, "-14") == 0 || strcmp(buf, "-0.35V") == 0)
            {
                I2C_Call(168);
            }
            else if (strcmp(buf, "-16") == 0 || strcmp(buf, "-0.40V") == 0)
            {
                I2C_Call(169);
            }
            else if (strcmp(buf, "-18") == 0 || strcmp(buf, "-0.45V") == 0)
            {
                I2C_Call(170);
            }
            else if (strcmp(buf, "-20") == 0 || strcmp(buf, "-0.50V") == 0)
            {
                I2C_Call(171);
            }
            else if (strcmp(buf, "-22") == 0 || strcmp(buf, "-0.55V") == 0)
            {
                I2C_Call(172);
            }
            else if (strcmp(buf, "-24") == 0 || strcmp(buf, "-0.60V") == 0)
            {
                I2C_Call(173);
            }
            else if (strcmp(buf, "+1") == 0 || strcmp(buf, "0.025V") == 0)
            {
                I2C_Call(177);
            }
            else if (strcmp(buf, "+2") == 0 || strcmp(buf, "0.05V") == 0)
            {
                I2C_Call(178);
            }
            else if (strcmp(buf, "+4") == 0 || strcmp(buf, "0.10V") == 0)
            {
                I2C_Call(179);
            }
            else if (strcmp(buf, "+6") == 0 || strcmp(buf, "0.15V") == 0)
            {
                I2C_Call(180);
            }
            else if (strcmp(buf, "+8") == 0 || strcmp(buf, "0.20V") == 0)
            {
                I2C_Call(181);
            }
            else if (strcmp(buf, "+10") == 0 || strcmp(buf, "0.25V") == 0)
            {
                I2C_Call(182);
            }
            else if (strcmp(buf, "+12") == 0 || strcmp(buf, "0.30V") == 0)
            {
                I2C_Call(183);
            }
            else if (strcmp(buf, "+14") == 0 || strcmp(buf, "0.35V") == 0)
            {
                I2C_Call(184);
            }
            else if (strcmp(buf, "+16") == 0 || strcmp(buf, "0.40V") == 0)
            {
                I2C_Call(185);
            }
            else if (strcmp(buf, "+18") == 0 || strcmp(buf, "0.45V") == 0)
            {
                I2C_Call(186);
            }
            else if (strcmp(buf, "+20") == 0 || strcmp(buf, "0.50V") == 0)
            {
                I2C_Call(187);
            }
            else if (strcmp(buf, "+22") == 0 || strcmp(buf, "0.55V") == 0)
            {
                I2C_Call(188);
            }
            else if (strcmp(buf, "+24") == 0 || strcmp(buf, "0.60V") == 0)
            {
                I2C_Call(189);
            }
    

            memset(buf, 0, sizeof buf);
            count = 0;
            
        
            data1 = 0b00111000;
            PORTF &= ~(1<<1); //Enable chip(active low)
            
            //Setting read/write settings for ADC
            //Sets to recieve data from channel 1
            spi_byte(0b00100000);
            spi_byte(0b00001100);
            spi_byte(0b00010000);
            spi_byte(0b01000000);
            
            //Read in data from ADC and output through uart commands
            spi_byte(data1);
            value = (spi_byte(0x00)<<8);
            value = 2*(value + spi_byte(0x00)-32768);
            
            v_out = 5.0/65536 * value;
            
            cali = Calibrate((1.225 - v_out) / 2750 * 1000000);
            
            i_out = fabs(cali);//change from 1.25 to 1.225 becasue the reference voltage is 2.45V.
            
           // i_out = Calibrate(i_out);
            
            sam[avg_count % 4] = i_out;
            
            i_avg = (sam[0]+sam[1]+sam[2]+sam[3])/4;
            
            memset(buf_v, 0, sizeof buf_v);
            memset(buf_i, 0, sizeof buf_i);
            memset(buf_avg, 0, sizeof buf_avg);
            
            double_char(buf_v, v_out, 2);
            double_char(buf_i, i_out, 2);
            double_char(buf_avg, i_avg, 2);
            
            //double_char(buf_i_n, i_out_n, 2);
            
//            sprintf(temp_str, "16 bit value is : %u\n\r", value);
            
 //           print(temp_str);


//            uart_print("Measure output voltage is : ");
//            print(buf_v);//Output string to terminal
//            uart_print(" V\n\r");


//            uart_print("Measure output current is : ");
//            if ((v_out > 1.25)&&(i_out > 0.01))
//            {
//                uart_print("-");
//            }
//            print(buf_i);//Output string to terminal
            uart_print("\n\r");


//            uart_print("Average current is : ");
            if (cali > 0)
            {
                uart_print("-");
            }
            print(buf_avg);//Output string to terminal
  

            
//          uart_print(" µA\n\r\n\r");
           
            avg_count++;
            
            
            PORTF |= (1<<1); //Disable chip
            PORTD ^= 1<<6; //LED on
            _delay_ms(500);
        }
        //PORTF |= (1<<1); //Disable chip
        //PORTD ^= 1<<6; //LED on
        //_delay_ms(500);
	}

}
