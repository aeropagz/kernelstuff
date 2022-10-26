#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/gpio.h>


#define PIN_NUM 17
#define MY_GPIO_INT_NAME "my_pin_int"
#define DEV_NAME "FANCY_DEVICE"

static short int pin_irq = 0;
unsigned long counter = 0;

static irqreturn_t pin_isr(int irq, void *data){
    counter++;
    return IRQ_HANDLED;
}

int init_gpio(void) {
    printk("Init Gpio\n");
    if(gpio_request(PIN_NUM, "myPin") < 0) return -1;
    if( (pin_irq = gpio_to_irq(PIN_NUM)) < 0) return -1;
    if( request_irq(pin_irq, pin_isr, IRQF_TRIGGER_RISING, MY_GPIO_INT_NAME, DEV_NAME )) return -1;
    return 0;
}

void clean_up_gpio(void){
    free_irq(pin_irq, DEV_NAME);
    gpio_free(PIN_NUM);
}
