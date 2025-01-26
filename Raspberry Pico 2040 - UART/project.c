/////////////////////////////////////////////

//Berk Muammer Kuzu - Anadologic
//Raspberry_Pico_2040_UART

/////////////////////////////////////////////

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>

//=============================UART configuration=============================//
#define UART_ID uart0
#define BAUD_RATE 115200
#define TX_PIN 0
#define RX_PIN 1
//============================================================================//

#define LED_PIN 25

#define HEADER 0xAB //Header
#define FOOTER 0x0F //Footer

void parse_uart_data(uint8_t header, uint8_t control_byte, uint8_t footer) {
    if (header != HEADER || footer != FOOTER) {
        printf("Invalid message\n");
        return;
    }
  
    bool led_enabled = control_byte & 0x01; // Extract LED enable/disable bit

    uint8_t frequency_code = (control_byte >> 1) & 0x0F; // Extract frequency from bits 1-4

    //=============================Frequency=============================//
    uint16_t delay_ms;
    switch (frequency_code) {
        case 0x00: delay_ms = 0; break;  // 0 Hz (LED off)
        case 0x01: delay_ms = 500; break;  // 2 Hz
        case 0x02: delay_ms = 250; break;  // 4 Hz
        case 0x03: delay_ms = 125; break;  // 8 Hz
        case 0x04: delay_ms = 62; break;   // 16 Hz
        case 0x05: delay_ms = 31; break;   // 32 Hz
        default:
            printf("Invalid frequency code\n");
            return;
    }
    //====================================================================//
  
    if (led_enabled && delay_ms > 0) {
        printf("LED Enabled. Blinking at frequency code: %d\n", frequency_code);
        while (true) {
            gpio_put(LED_PIN, 1);
            sleep_ms(delay_ms);
            gpio_put(LED_PIN, 0);
            sleep_ms(delay_ms);
            if (uart_is_readable(UART_ID)) {
                break;
            }
        }
    } else {
        printf("LED Disabled or 0 Hz frequency\n");
        gpio_put(LED_PIN, 0);  // Ensure LED is off
    }
}

int main() {
  
    stdio_init_all();
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RX_PIN, GPIO_FUNC_UART);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    printf("UART LED Controller Initialized\n");

    while (true) {
        if (uart_is_readable(UART_ID)) {
            uint8_t header = uart_getc(UART_ID);
            uint8_t control_byte = uart_getc(UART_ID);
            uint8_t footer = uart_getc(UART_ID);

            parse_uart_data(header, control_byte, footer);
        }
    }
    return 0;
}
