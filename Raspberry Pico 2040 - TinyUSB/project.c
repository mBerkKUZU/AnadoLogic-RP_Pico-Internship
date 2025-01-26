/////////////////////////////////////////////

//Berk Muammer Kuzu - Anadologic
//Raspberry_Pico_2040_TinyUSB

//------------------------------------------------------------------------------------------------------//
https://github.com/hathach/tinyusb 
//------------------------------------------------------------------------------------------------------//

/////////////////////////////////////////////
#include "pico/stdlib.h"
#include "tusb.h"

#define LED_PIN 25  // Onboard LED pin

// Global variables to control LED blinking
volatile bool enable_blink = false;
volatile int frequency = 0;

// Function to blink the LED at the specified frequency
void blink_led(int frequency) {
    static absolute_time_t next_blink_time;
    static bool led_state = false;

    if (absolute_time_diff_us(get_absolute_time(), next_blink_time) <= 0) {
        led_state = !led_state;
        gpio_put(LED_PIN, led_state);
        next_blink_time = make_timeout_time_us(1000000 / frequency);
    }
}

// Callback for USB CDC data reception
void tud_cdc_rx_cb(uint8_t itf) {
    uint8_t buf[64];
    uint32_t count = tud_cdc_read(buf, sizeof(buf));

    // Ensure we have at least 4 bytes for a valid packet
    if (count >= 4) {
        // Check header and footer
        if (buf[0] == 0xBA && buf[3] == 0x0F) {
            uint8_t enable = buf[1];
            uint8_t frequency_byte = buf[2];

            if (enable == 0x01) {
                enable_blink = true;
                if (frequency_byte == 0x01) {
                    frequency = 2;  // 2 Hz
                    tud_cdc_write_str("Blinking at 2 Hz\n");
                } else if (frequency_byte == 0x02) {
                    frequency = 8;  // 8 Hz
                    tud_cdc_write_str("Blinking at 8 Hz\n");
                } else {
                    frequency = 0;  // Invalid frequency
                    tud_cdc_write_str("Invalid frequency\n");
                }
            } else if (enable == 0x00) {
                enable_blink = false;
                frequency = 0;
                gpio_put(LED_PIN, 0);
                tud_cdc_write_str("LED disabled\n");
            } else {
                tud_cdc_write_str("Invalid enable byte\n");
            }
        } else {
            tud_cdc_write_str("Invalid header or footer\n");
        }
        tud_cdc_write_flush();
    }
}

int main() {
    stdio_init_all();
    tusb_init();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    while (true) {
        tud_task();  // Handle USB events

        if (enable_blink && frequency > 0) {
            blink_led(frequency);
        }
    }
}
