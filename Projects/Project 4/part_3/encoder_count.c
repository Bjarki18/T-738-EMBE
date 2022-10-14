#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/gpio.h>                       // for the GPIO functions 
#include <linux/interrupt.h>                  // for the IRQ code

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Derek Molloy"); 
MODULE_DESCRIPTION("A Encoder/LED test driver for the RPi"); 
MODULE_VERSION("0.1");

static unsigned int gpioEncoder1 = 21;             // pin 11 (GPIO17) 
static unsigned int gpioEncoder2 = 20;          // pin 13 (GPIO27) 
static unsigned int irqNumber;                // share IRQ num within file 
static unsigned int encoderCounter = 0;        // store number of encoder ticks

module_param(encoderCounter, int, S_IRUGO);
MODULE_PARM_DESC(encoderCounter, "The current counter value");


// prototype for the custom IRQ handler function, function below 
static irq_handler_t  erpi_gpio_irq_handler(unsigned int irq, 
                                            void *dev_id, struct pt_regs *regs);


static int __init erpi_gpio_init(void) 
{
    int result = 0;
    printk(KERN_INFO "GPIO_TEST: Initializing the GPIO_TEST LKM\n");

    gpio_request(gpioEncoder1, "sysfs");          // request LED GPIO
    gpio_direction_input(gpioEncoder1);         // set up as input 
    gpio_export(gpioEncoder1, false);             // appears in /sys/class/gpio
                                                // false prevents in/out change   
    gpio_request(gpioEncoder2, "sysfs");       // set up gpioButton   
    gpio_direction_input(gpioEncoder2);        // set up as input   
    gpio_export(gpioEncoder2, false);          // appears in /sys/class/gpio

    irqNumber = gpio_to_irq(gpioEncoder1);     // map GPIO to IRQ number
    printk(KERN_INFO "GPIO_TEST: button mapped to IRQ: %d\n", irqNumber);

    // This next call requests an interrupt line   
    result = request_irq(irqNumber,          // interrupt number requested            
        (irq_handler_t) erpi_gpio_irq_handler,   // handler function            
        IRQF_TRIGGER_RISING,                   // on rising edge (press, not release)            
        "erpi_gpio_handler",                     // used in /proc/interrupts
        NULL);                                   // *dev_id for shared interrupt lines
    printk(KERN_INFO "GPIO_TEST: IRQ request result is: %d\n", result);
    return result;
}

static void __exit erpi_gpio_exit(void) 
{   
    printk(KERN_INFO "Exiting kernel module \n");

    // NULL THE VALUES
    gpio_set_value(gpioEncoder1, 0);
    gpio_set_value(gpioEncoder2, 0);
    // UNEXPORT THE PINS
    gpio_unexport(gpioEncoder1);
    gpio_unexport(gpioEncoder2);
    // FREE THE UNITS
    free_irq(irqNumber, NULL);
    gpio_free(gpioEncoder1);
    gpio_free(gpioEncoder2);
    encoderCounter = 0;
}

static irq_handler_t erpi_gpio_irq_handler(unsigned int irq, 
                                           void *dev_id, struct pt_regs *regs) 
{   
    int val = gpio_get_value(gpioEncoder1);
    if (val == 1){
        encoderCounter++;
    }else{
        encoderCounter--;
    } 
    printk(KERN_INFO "Encoder count: %d\n", encoderCounter);
    // numberPresses++;                         // global counter
    return (irq_handler_t) IRQ_HANDLED;      // announce IRQ handled 
}

module_init(erpi_gpio_init);
module_exit(erpi_gpio_exit);