#ifndef _DS18B20_H
#define _DS18B20_H

#ifdef _DS18B20_DEFINITIONS_FILE
#include "ds18b20_definitions.h"
#endif

#ifndef DS18B20_DDR
#define DS18B20_DDR  DDRA
#endif

#ifndef DS18B20_PORT
#define DS18B20_PORT PORTA
#endif

#ifndef DS18B20_PIN
#define DS18B20_PIN  PINA
#endif

#ifndef DS18B20
#define DS18B20      PA0
#endif

/* Service functions */
extern char crc8_MAXIM(char *data, uint8_t len);

/* Low-level functions */
extern char   DS18B20_init(void);

/* ROM-level functions */
extern void   DS18B20_read_ROM(char *rom);
extern void   DS18B20_skip_ROM(void);

/* Command-level functions */
extern void   DS18B20_convert_T(void);
extern void   DS18B20_read_scratchpad(char *scratchpad);
extern void   DS18B20_write_scratchpad(char *scratchpad);

#endif
