#include <eeprom_93c86.h> // https://github.com/vtl/eeprom_93c86

#define EEPROM_MAGIC_0      0xAA
#define EEPROM_MAGIC_1      0x55

#define PERSISTENT_COUNTER  10

int eeprom_ready = 0;

void setup_eeprom(void)
{
  unsigned char magic_0, magic_1;

  eeprom_init(11, 12, 9, 8); // CS, CLK, DI, DO pins
  eeprom_ewen();             // write enable

 // Detect EEPROM presence by reading two magic bytes 

  for (int i = 0; i < 2; i++) {
    magic_0 = eeprom_read(0);
    magic_1 = eeprom_read(1);
    if ((magic_0 == EEPROM_MAGIC_0) &&
        (magic_1 == EEPROM_MAGIC_1)) {
        eeprom_ready = 1;
        break;
    } else {                 // try to init EEPROM
       eeprom_write(0, EEPROM_MAGIC_0);
       eeprom_write(1, EEPROM_MAGIC_1);
    }
  }
  if (eeprom_ready)
    Serial.println("EEPROM OK");
  else
    Serial.println("EEPROM was not found");
}

unsigned char eeprom_load(int address, unsigned char def)
{
  unsigned char value;

  if (!eeprom_ready) // no EEPROM, return default value
    return def;

  value = eeprom_read(address);

  return value;
}

void eeprom_store(int address, unsigned char value)
{
  if (!eeprom_ready)
    return;

  eeprom_write(address, value);
}

void setup()
{
  Serial.begin(9600);
  setup_eeprom();
}

void loop()
{
  unsigned char counter;

  counter = eeprom_load(PERSISTENT_COUNTER, 0);
  Serial.println(counter);
  counter++;
  eeprom_store(PERSISTENT_COUNTER, counter);
  delay(1000);
}

