/* Simple example for Teensy USB Development Board
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2008 PJRC.COM, LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include "usb_serial.h"

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

void send_str(const char *s);
void send_int(unsigned int val);
uint8_t recv_str(char *buf, uint8_t size);

volatile unsigned int system_timer = 0;

int main(void)
{
	int changed = 0, val;
	int port = 'B' - 'A';
	int pin = 1 << ('0' - '0');
	unsigned int timer_prev = 1;

	system_timer = 100;

	CPU_PRESCALE(0x00); // 16 MHz
	usb_init();
	while (!usb_configured()) /* wait */ ;
	_delay_ms(1000);
	CPU_PRESCALE(0x04); // 1 MHz
	TCCR1B |= (1 << CS10); // Set timer1 to 1 microsecond steps
	TIMSK1 |= (1 << TOIE1); // enabled timer overflow interrupt;
	sei();

	while (1) {
		if (system_timer != timer_prev) {
			timer_prev++;
//			send_int((int)system_timer);
			usb_serial_putchar('\n');
			continue;
		}
	}
}

ISR(TIMER1_OVF_vect)
{
	system_timer++;
}

// Send a string to the USB serial port.  The string must be in
// flash memory, using PSTR
//
void send_str(const char *s)
{
	char c;
	while (1) {
		c = pgm_read_byte(s++);
		if (!c) break;
		usb_serial_putchar(c);
	}
}

/* Write a decimal integer to the serial port
 */
void send_int(unsigned int val)
{
	char c_out[10];
	int ptr;
	c_out[ptr++] = '\0';
	if (val < 0) {
		c_out[ptr++] = '-';
		val = -val;
	}
	while (val > 0) {
		c_out[ptr++] = (val % 10) + '0';
		val /= 10;
	}
	ptr--;
	while (ptr) {
		usb_serial_putchar(c_out[ptr--]);
	}
}


