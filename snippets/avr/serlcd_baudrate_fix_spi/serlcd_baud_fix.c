/**
 * @file serlcd_baudrate_fix.c
 * @brief AVR SPI utility for sending a SerLCD baud-rate/config command.
 *
 * This snippet initializes SPI0 as a master and sends a two-byte command
 * sequence to a SparkFun SerLCD-style display backpack.
 *
 * Target style:
 * - AVR DB-series register naming
 * - SPI0 on PORTA
 *
 * Pin mapping:
 * - PA4 = MOSI
 * - PA5 = MISO
 * - PA6 = SCK
 * - PA7 = SS / CS
 */

#define F_CPU 4000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/* ---------- Pin Definitions ---------- */

#define SPI_PORT        PORTA
#define SPI_MOSI_bm     PIN4_bm
#define SPI_MISO_bm     PIN5_bm
#define SPI_SCK_bm      PIN6_bm
#define SPI_SS_bm       PIN7_bm

/* ---------- SPI Helpers ---------- */

static void spi0_cs_low(void)
{
    SPI_PORT.OUTCLR = SPI_SS_bm;
}

static void spi0_cs_high(void)
{
    SPI_PORT.OUTSET = SPI_SS_bm;
}

/**
 * @brief Initialize SPI0 in master mode.
 */
static void spi0_init_master(void)
{
    /*
     * Configure SPI pins:
     * MOSI, SCK, and SS are outputs.
     * MISO is input.
     */
    SPI_PORT.DIRSET = SPI_MOSI_bm | SPI_SCK_bm | SPI_SS_bm;
    SPI_PORT.DIRCLR = SPI_MISO_bm;

    /*
     * Keep chip select inactive before communication.
     */
    spi0_cs_high();

    /*
     * Enable SPI0 as master.
     * Prescaler DIV4.
     * Mode 0 by default because CTRLB = 0.
     */
    SPI0.CTRLA = SPI_ENABLE_bm |
                 SPI_MASTER_bm |
                 SPI_PRESC_DIV4_gc;

    SPI0.CTRLB = 0;
}

/**
 * @brief Send one byte over SPI0.
 *
 * @param data Byte to transmit.
 */
static void spi0_send_byte(uint8_t data)
{
    SPI0.DATA = data;

    /*
     * Wait until SPI transfer is complete.
     */
    while (!(SPI0.INTFLAGS & SPI_IF_bm)) {
        ;
    }

    /*
     * Clear interrupt flag by writing 1.
     */
    SPI0.INTFLAGS = SPI_IF_bm;
}

/**
 * @brief Send SerLCD command sequence.
 *
 * The command sequence used here is:
 * - '|'  = command prefix
 * - 0x0D = command value
 */
static void serlcd_send_baudrate_fix_command(void)
{
    /*
     * Keep CS low for the whole command sequence instead of toggling
     * CS between the prefix byte and command byte.
     */
    spi0_cs_low();

    spi0_send_byte('|');
    spi0_send_byte(0x0D);

    spi0_cs_high();
}

int main(void)
{
    spi0_init_master();

    /*
     * Send the command once on startup.
     */
    serlcd_send_baudrate_fix_command();

    while (1)
    {
        /*
         * Nothing else needed.
         * Utility only sends the command once.
         */
        _delay_ms(500);
    }
}