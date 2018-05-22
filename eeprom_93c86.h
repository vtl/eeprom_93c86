/*
 *  Arduino 93C86 EEPROM interface
 *
 *  (C) 2018 Vitaly Mayatskikh <vitaly@gravicappa.info>
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

#ifndef _EEPROM_93C86_H_
#define _EEPROM_93C86_H_

#ifdef __cplusplus
extern "C" {
#endif

void eeprom_init(int _cs, int _clk, int _di, int _do);
void eeprom_ewen();
void eeprom_ewds();
unsigned char eeprom_read(int address);
void eeprom_write(int address, unsigned char data);
void eeprom_erase(int address);

#ifdef __cplusplus
}
#endif

#endif
