#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/spi.h"

/*
|SPI RX  |GP4|
|SPI CSn |GP5|
|SPI SCK |GP6|
|SPI TX  |GP7|
|   RST  |GP21|

*/

int main(){
    stdio_init_all();
    spi_init(spi0, 500*1000);
    gpio_set_function(4, GPIO_FUNC_SPI);
    gpio_set_function(6, GPIO_FUNC_SPI);
    gpio_set_function(7, GPIO_FUNC_SPI);
    gpio_init(5);
    gpio_init(21);
    gpio_set_dir(5 , GPIO_OUT);
    gpio_set_dir(21, GPIO_OUT);
    gpio_init(5);
    gpio_init(21);
}