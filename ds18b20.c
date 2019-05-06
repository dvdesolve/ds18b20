#include <avr/io.h>
#include <util/delay.h>

#include "ds18b20.h"

/*
 * Service functions
 *
*/

char crc8_MAXIM(char *data, uint8_t len) {
    // Dallas/Maxim CRC-8 checksum
    // Init: 0x00
    // Poly: 0x8C (reversed)
    // Check: 0xA1 ("123456789")
    //

    char crc = 0x00;

    while (len--) {
        crc ^= *data++;

        for (uint8_t i = 0; i < 8; i++)
            crc = crc & 0x01 ? (crc >> 1) ^ 0x8C : (crc >> 1);
    }

    return crc;
}

/*
 * Low-level functions
 *
*/

char DS18B20_init(void) {
    // Initializes DS18B20 so it becomes ready to serve requests
    //

    /* reset pulse */
    DS18B20_DDR |= (1 << DS18B20); // enable Master Tx
    DS18B20_PORT &= ~(1 << DS18B20); // hold the bus LOW
    _delay_us(500);

    /* presence pulse */
    DS18B20_DDR &= ~(1 << DS18B20); // enable Master Rx
    _delay_us(65);

    if (DS18B20_PIN & (1 << DS18B20)) { // init failed
        _delay_us(435);
        return 0;
    }
    else { // init successfull
        _delay_us(435);
        return 1;
    }
}

char DS18B20_read_slot(void) {
    // Reads one bit (slot in MAXIM terminology) from DS18B20
    //

    char state = 0;

    /* start read slot */
    DS18B20_DDR |= (1 << DS18B20); // enable Master Tx
    DS18B20_PORT &= ~(1 << DS18B20); // hold the bus LOW
    _delay_us(5);

    /* read signal */
    DS18B20_DDR &= ~(1 << DS18B20); // enable Master Rx
    _delay_us(5);

    if (DS18B20_PIN & (1 << DS18B20)) // read logic "1"
        state = 1;

    /* finish read slot + recovery */
    _delay_us(65);

    return state;
}

/*
 * Byte-level functions
 *
*/

void DS18B20_write_byte(char byte) {
    // Writes one byte to DS18B20
    //

    for (uint8_t i = 0; i < 8; i++) {
        /* start write slot */
        DS18B20_DDR |= (1 << DS18B20); // enable Master Tx
        DS18B20_PORT &= ~(1 << DS18B20); // hold the bus LOW

        if (byte & 0x1) { // write logic "1"
            _delay_us(5);
            DS18B20_DDR &= ~(1 << DS18B20); // release the bus
            _delay_us(65);
        }
        else { // write logic "0"
            _delay_us(70);
            DS18B20_DDR &= ~(1 << DS18B20); // release the bus
        }

        /* recovery */
        _delay_us(5);

        byte >>= 1; // shift byte
    }
}

char DS18B20_read_byte(void) {
    // Reads one byte from DS18B20
    //

    char byte = 0x0;

    for (uint8_t i = 0; i < 8; i++) {
        byte >>= 1; // shift current byte

        if (DS18B20_read_slot())
            byte |= 0x80;
    }

    return byte;
}

/*
 * ROM-level functions
 *
*/

void DS18B20_read_ROM(char *rom) {
    // Reads the entire ROM contents
    //

    DS18B20_write_byte(0x33); // Read Rom

    for (uint8_t i = 0; i < 8; i++)
        rom[i] = DS18B20_read_byte();
}

void DS18B20_skip_ROM(void) {
    // Skips ROM reading
    //

    DS18B20_write_byte(0xCC); // Skip Rom
}

/*
 * Command-level functions
 *
*/

void DS18B20_convert_T(void) {
    // Starts temperature conversion
    //

    DS18B20_write_byte(0x44); // Convert T
}

void DS18B20_read_scratchpad(char *scratchpad) {
    // Reads the entire scratchpad contents
    //

    DS18B20_write_byte(0xBE); // Read Scratchpad

    for (uint8_t i = 0; i < 9; i++)
        scratchpad[i] = DS18B20_read_byte();
}

void DS18B20_write_scratchpad(char *scratchpad) {
    // Writes user bytes (TH, TL and config) to the scratchpad
    //

    DS18B20_write_byte(0x4E); // Write Scratchpad

    for (uint8_t i = 0; i < 3; i++)
        DS18B20_write_byte(scratchpad[i]);
}
