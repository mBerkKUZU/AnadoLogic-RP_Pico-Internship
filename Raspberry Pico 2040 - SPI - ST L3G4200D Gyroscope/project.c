/////////////////////////////////////////////

//Berk Muammer Kuzu - Anadologic
//Raspberry_Pico_2040_SPI_Gyro

//The Pmod GYRO utilizes ST L3G4200D gyroscope to provide motion sensing data on each of the three Cartesian axes. 
//Users may configure both the resolution and filtering options for the measured data.

//------------------------------------------------------------------------------------------------------//
//https://digilent.com/reference/_media/reference/pmod/pmodgyro/stmicroelectronics-l3g4200d-datasheet.pdf
//------------------------------------------------------------------------------------------------------//

//WHO_AM_I (0Fh)
//CTRL_REG1 (20h)
//OUT_X_L (28h), OUT_X_H (29h) : X-axis angular rate data. The value is expressed as two’s complement.
//OUT_Y_L (2Ah), OUT_Y_H (2Bh) : Y-axis angular rate data. The value is expressed as two’s complement.
//OUT_Z_L (2Ch), OUT_Z_H (2Dh) : Z-axis angular rate data. The value is expressed as two’s complement.



/////////////////////////////////////////////
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdio.h>

//Led Pin
#define LED_PIN 25

// Define SPI pins and settings
#define SPI_PORT spi0
#define PIN_MISO 4  // GPIO4
#define PIN_CS   5  // GPIO1
#define PIN_SCK  2  // GPIO2
#define PIN_MOSI 3  // GPIO3

#define WHO_AM_I 0x0F  // Register to verify communication
#define CTRL_REG1 0x20 // Control register to enable the sensor
#define OUT_X_L  0x28  // X-axis data output (low byte)
#define OUT_X_H  0x29  // X-axis data output (high byte)
#define OUT_Y_L  0x2A  // Y-axis data output (low byte)
#define OUT_Y_H  0x2B  // Y-axis data output (high byte)
#define OUT_Z_L  0x2C  // Z-axis data output (low byte)
#define OUT_Z_H  0x2D  // Z-axis data output (high byte)

void cs_select() {
    gpio_put(PIN_CS, 0); // Set CS low
}

void cs_deselect() {
    gpio_put(PIN_CS, 1); // Set CS high
}

uint8_t spi_read_register(uint8_t reg) {
    uint8_t tx[2] = {reg | 0x80, 0x00}; // Read command (MSB set)
    uint8_t rx[2] = {0};

    cs_select();
    spi_write_read_blocking(SPI_PORT, tx, rx, 2);
    cs_deselect();

    return rx[1];
}

void spi_write_register(uint8_t reg, uint8_t value) {
    uint8_t tx[2] = {reg & 0x7F, value}; // Write command (MSB cleared)

    cs_select();
    spi_write_blocking(SPI_PORT, tx, 2);
    cs_deselect();
}

void read_gyro_data(int16_t *x, int16_t *y, int16_t *z) {
    uint8_t xl = spi_read_register(OUT_X_L);
    uint8_t xh = spi_read_register(OUT_X_H);
    *x = (int16_t)((xh << 8) | xl);

    uint8_t yl = spi_read_register(OUT_Y_L);
    uint8_t yh = spi_read_register(OUT_Y_H);
    *y = (int16_t)((yh << 8) | yl);

    uint8_t zl = spi_read_register(OUT_Z_L);
    uint8_t zh = spi_read_register(OUT_Z_H);
    *z = (int16_t)((zh << 8) | zl);
}

int main() {
    stdio_init_all();

    // Initialize SPI with correct format (Mode 1: CPOL=0, CPHA=1)
    spi_init(SPI_PORT, 1000 * 1000); // 1 MHz
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);

    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    cs_deselect();

    // Verify communication
    uint8_t who_am_i = spi_read_register(WHO_AM_I);
    printf("WHO_AM_I register: 0x%02X\n", who_am_i);

    // Enable the gyroscope (normal mode, all axes enabled, 100 Hz)
    spi_write_register(CTRL_REG1, 0x0F);

    int16_t x, y, z;


    while (true) {
        read_gyro_data(&x, &y, &z);
        printf("Gyro Data - X: %d, Y: %d, Z: %d\n", x, y, z);
        sleep_ms(500); // Delay for readability
    }
}

