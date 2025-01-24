/////////////////////////////////////////////

//Berk Muammer Kuzu - Anadologic
//Raspberry_Pico_2040_I2C_Gyro


//The Pmod GYRO utilizes ST L3G4200D gyroscope to provide motion sensing data on each of the three Cartesian axes. 
//Users may configure both the resolution and filtering options for the measured data.

//------------------------------------------------------------------------------------------------------//
//https://digilent.com/reference/_media/reference/pmod/pmodgyro/stmicroelectronics-l3g4200d-datasheet.pdf
//------------------------------------------------------------------------------------------------------//

/////////////////////////////////////////////
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>

#define I2C_PORT i2c0
#define SDA_PIN 4
#define SCL_PIN 5

#define L3G4200D_ADDR 0x69  // I2C address of the gyroscope

#define GYRO_SENSITIVITY1 0.00875
#define GYRO_SENSITIVITY2 0.0175
#define GYRO_SENSITIVITY3 0.070

// L3G4200D register addresses
#define WHO_AM_I 0x0F
#define CTRL_REG1 0x20
#define OUT_X_L 0x28

int GYRO_SENSITIVITY (char *user_input, int *gyro_sens_selec){

    switch(*user_input){
        case '1': return *gyro_sens_selec = 1;
        case '2': return *gyro_sens_selec = 2;
        case '3': return *gyro_sens_selec = 3;
    }

}

void write_register(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    i2c_write_blocking(I2C_PORT, L3G4200D_ADDR, data, 2, false);
}

uint8_t read_register(uint8_t reg) {
    uint8_t value;
    i2c_write_blocking(I2C_PORT, L3G4200D_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, L3G4200D_ADDR, &value, 1, false);
    return value;
}

void read_gyro_data(int16_t *x, int16_t *y, int16_t *z, float *x_rate, float *y_rate, float *z_rate, int *gyro_sens_selec) {
    uint8_t buffer[6];
    uint8_t reg = OUT_X_L | 0x80;  // Set auto-increment bit
    i2c_write_blocking(I2C_PORT, L3G4200D_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, L3G4200D_ADDR, buffer, 6, false);

    // Combine high and low bytes for each axis
    *x = (int16_t)((buffer[1] << 8) | buffer[0]);
    *y = (int16_t)((buffer[3] << 8) | buffer[2]);
    *z = (int16_t)((buffer[5] << 8) | buffer[4]);

    switch(*gyro_sens_selec) {
        case 1:
            *x_rate = *x * GYRO_SENSITIVITY1;
            *y_rate = *y * GYRO_SENSITIVITY1;
            *z_rate = *z * GYRO_SENSITIVITY1;
            break;
        case 2:
            *x_rate = *x * GYRO_SENSITIVITY2;
            *y_rate = *y * GYRO_SENSITIVITY2;
            *z_rate = *z * GYRO_SENSITIVITY2;
            break;
        case 3:
            *x_rate = *x * GYRO_SENSITIVITY3;
            *y_rate = *y * GYRO_SENSITIVITY3;
            *z_rate = *z * GYRO_SENSITIVITY3;
            break;
        default:
            *x_rate = *x * GYRO_SENSITIVITY3;
            *y_rate = *y * GYRO_SENSITIVITY3;
            *z_rate = *z * GYRO_SENSITIVITY3;
            break;
    }
}

int main() {
    // Initialize I2C
    i2c_init(I2C_PORT, 400 * 1000);  // I2C frequency
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    int gyro_sens_selec = 3, i = 1;
    float x_rate, y_rate, z_rate;
    int16_t x, y, z;

    stdio_init_all();

    // Check WHO_AM_I register
    uint8_t who_am_i = read_register(WHO_AM_I);
    if (who_am_i != 0xD3) {
        printf("Invalid WHO_AM_I: 0x%02X\n", who_am_i);
        return 1;
    }
    printf("L3G4200D detected!\n");

    // Configure the gyroscope
    write_register(CTRL_REG1, 0x0F);  // Enable X, Y, Z and set data rate

        // Check for user input
        if (i == 1) {
            scanf("%d", gyro_sens_selec);
            char user_input = getchar();
            if (user_input == 'q') {
                printf("Exiting program...\n");
                return 1;  // Break the loop if user presses 'q'
            }else if(user_input == '1'){

                while (1) {
                    
                    GYRO_SENSITIVITY(&user_input, &gyro_sens_selec);

                    // Read gyroscope data
                    read_gyro_data(&x, &y, &z, &x_rate, &y_rate, &z_rate, &gyro_sens_selec);
    
                    // Print angular velocity data
                    printf("Gyro Data: X=%d, Y=%d, Z=%d\n", x, y, z);
                    printf("Angular Rates - X: %.2f °/s, Y: %.2f °/s, Z: %.2f °/s\n", x_rate, y_rate, z_rate);
    
                    sleep_ms(500);  // Delay for readability
                }

            }else if(user_input == '2'){

                while (1) {
                    
                    GYRO_SENSITIVITY(&user_input, &gyro_sens_selec);

                    // Read gyroscope data
                    read_gyro_data(&x, &y, &z, &x_rate, &y_rate, &z_rate, &gyro_sens_selec);
    
                    // Print angular velocity data
                    printf("Gyro Data: X=%d, Y=%d, Z=%d\n", x, y, z);
                    printf("Angular Rates - X: %.2f °/s, Y: %.2f °/s, Z: %.2f °/s\n", x_rate, y_rate, z_rate);
    
                    sleep_ms(500);
                }

            }else if(user_input == '3'){

                while (1) {
                    
                    GYRO_SENSITIVITY(&user_input, &gyro_sens_selec);

                    // Read gyroscope data
                    read_gyro_data(&x, &y, &z, &x_rate, &y_rate, &z_rate, &gyro_sens_selec);
    
                    // Print angular velocity data
                    printf("Gyro Data: X=%d, Y=%d, Z=%d\n", x, y, z);
                    printf("Angular Rates - X: %.2f °/s, Y: %.2f °/s, Z: %.2f °/s\n", x_rate, y_rate, z_rate);
    
                    sleep_ms(500);  // Delay for readability
                }

            }else{
                printf("Invalid !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            }
        }

    return 0;
}
