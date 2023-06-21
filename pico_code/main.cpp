#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
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

// version reg 0x37


int main(){
    const uint8_t version_reg[] ={0x80 | (0x37<<1), 0}; 
    uint8_t dst_reg[2];
    stdio_init_all();
    spi_init(spi0, 1000*1000);
    gpio_set_function(4, GPIO_FUNC_SPI);
    gpio_set_function(6, GPIO_FUNC_SPI);
    gpio_set_function(7, GPIO_FUNC_SPI);
    bi_decl(bi_3pins_with_func(4, 7, 6, GPIO_FUNC_SPI));
    gpio_init(5);
    gpio_set_dir(5, GPIO_OUT);
    gpio_put(5, true);
    bi_decl(bi_1pin_with_name(5, "SPI CS"));
    gpio_init(21);
    gpio_set_dir(21, GPIO_IN);
    asm volatile("nop \n nop \n nop");
    bool hardrst = gpio_get(21);
    gpio_set_dir(21, GPIO_OUT);
    gpio_put(21, false);
    sleep_us(2);
    gpio_put(21, true);
    sleep_ms(6000);
    gpio_put(5, false);
    asm volatile("nop \n nop \n nop");
    //spi_write_blocking(spi0, &version_reg[0],1);
    spi_write_read_blocking(spi0, &version_reg[0], dst_reg, 2);
    //sleep_ms(10);
    //spi_read_blocking(spi0, 0, dst_reg, 1);
    asm volatile("nop \n nop \n nop");
    gpio_put(5, true);
    printf("%x-%x", int(dst_reg[0]), int(dst_reg[1]));


    return(0);
}