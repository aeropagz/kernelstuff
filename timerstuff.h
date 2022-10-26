#include <linux/time.h>



static void my_callback(struct timer_list* timmi)
{
    mytimer.expires=timmi->expires+1*HZ;	
    printk("Callback wurde aufgerufen, expires ist %lu\n",timmi->expires);	
    add_timer(&mytimer);
}
