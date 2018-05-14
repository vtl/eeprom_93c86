/*
 *  Arduino 93C86 EEPROM interface
 *
 *  (C) 2018 Vitaly Mayatskikh <vitaly@gravicappa.info>
 *
 *  93C86 datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/21797L.pdf
 *
 *  CS    1         8   Vcc
 *  CLK   2  93C86  7   R10k->Vcc
 *  DI    3         6   GND
 *  DO    4         5   GND
 *
 *  Chip starts with writes disabled
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Arduino.h"
#include "eeprom_93c86.h"

#define EEPROM_CS  11
#define EEPROM_CLK 12
#define EEPROM_DI   9
#define EEPROM_DO   8

enum {
     OP_READ,
     OP_EWEN,
     OP_ERASE,
     OP_ERAL,
     OP_WRITE,
     OP_WRAL,
     OP_EWDS
};

struct opcode {
     const char *name;
     int opcode;
     int opcode_len;
     int addr_len;
     int di_len;
     int do_len;
     int clk;
     int rdy;
};

struct opcode ops[] = {
     { "READ",  0b10,   2, 11, 0, 8, 22, 0},
     { "EWEN",  0b0011, 4,  9, 0, 0, 14, 0},
     { "ERASE", 0b11,   2, 11, 0, 0, 14, 1},
     { "ERAL",  0b0010, 4,  9, 0, 0, 14, 1},
     { "WRITE", 0b01,   2, 11, 8, 0, 22, 1},
     { "WRAL",  0b0001, 4,  9, 8, 0, 22, 1},
     { "EWDS",  0b0000, 4,  9, 0, 0, 14, 0}
};

void eeprom_clk(void)
{
     digitalWrite(EEPROM_CLK, HIGH);
     delayMicroseconds(1);
     digitalWrite(EEPROM_CLK, LOW);
     delayMicroseconds(1);
}

bool eeprom_recv_bit(void)
{
     bool data;

     digitalWrite(EEPROM_CLK, HIGH);
     delayMicroseconds(1);
     data = digitalRead(EEPROM_DO);
     digitalWrite(EEPROM_CLK, LOW);
     delayMicroseconds(1);
     return data;
}

void eeprom_send_bit(bool bit)
{
     digitalWrite(EEPROM_DI, bit);
     eeprom_clk();
}

void eeprom_send_bits(unsigned int data, unsigned char len)
{
     if (len == 0)
	  return;

     for (int i = len - 1; i >= 0; i--) {
	  eeprom_send_bit(!!(data & (1 << i)));
     }
}

unsigned int eeprom_recv_bits(unsigned char len)
{
     unsigned int data = 0;

     if (len == 0)
	  return 0;

     for (int i = 0; i < len; i++) {
	  data |= eeprom_recv_bit() << (len - i - 1);
     }

     return data;
}

void eeprom_cs(bool en)
{
     digitalWrite(EEPROM_CS, en);
     if (en) {
	  digitalWrite(EEPROM_DI, HIGH);
	  eeprom_clk();
     }
}

int eeprom_do_op(int opcode, int addr, unsigned char data)
{
     struct opcode *op = &ops[opcode];
     int i;
     int recv;

     pinMode(EEPROM_CS,  OUTPUT);
     pinMode(EEPROM_CLK, OUTPUT);
     pinMode(EEPROM_DI,  OUTPUT);
     pinMode(EEPROM_DO,  INPUT);

     eeprom_cs(HIGH);
     eeprom_send_bits(op->opcode, op->opcode_len);
     eeprom_send_bits(addr, op->addr_len);
     eeprom_send_bits(data, op->di_len);
     recv = eeprom_recv_bits(op->do_len);
     if (op->rdy) {
	  for (int i = 0; i < 5000; i++) {
	       if (digitalRead(EEPROM_DO))
		    break;
	       delayMicroseconds(1);
	  }
     }
     eeprom_cs(LOW);
     return recv;
}

void eeprom_ewen()
{
     eeprom_do_op(OP_EWEN, 0, 0);
}

void eeprom_ewds()
{
     eeprom_do_op(OP_EWDS, 0, 0);
}

unsigned char eeprom_read(int address)
{
     return eeprom_do_op(OP_READ, address, 0);
}

void eeprom_write(int address, unsigned char data)
{
     eeprom_do_op(OP_WRITE, address, data);
}

void eeprom_erase(int address)
{
     eeprom_do_op(OP_ERASE, address, 0);
}
