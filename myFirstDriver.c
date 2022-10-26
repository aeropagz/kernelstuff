#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/timer.h>

#include "gpio.h"

#define DEV_NAME "FANCY_DEVICE"

static dev_t myDevNumber;
static struct cdev *myCdev;
static struct class *myClass;
static struct device *myDevice;

static struct timer_list mytimer;

static unsigned long hits_per_second = 0;

static void my_callback(struct timer_list* timmi)
{
    printk("Hits: %ld\n", hits_per_second);
    printk("Counter: %ld", counter);
    hits_per_second = counter;
	mytimer.expires=timmi->expires+1*HZ;	
	printk("Callback wurde aufgerufen, expires ist %lu\n",timmi->expires);	
	add_timer(&mytimer);
    counter = 0;
}

static ssize_t my_drv_read(struct file *instance, char  __user *user, size_t count, loff_t *offset)
{
    unsigned long not_copied, to_copy;
    to_copy=min(count,sizeof(hits_per_second));
    not_copied=copy_to_user(user, &hits_per_second, to_copy);
    *offset += to_copy-not_copied;
    printk("Habe %ld Bytes geschrieben\n",to_copy-not_copied);
    printk("Hits: %ld", hits_per_second);
    return to_copy-not_copied; 
}	 

static struct file_operations fops={
    .owner=THIS_MODULE,
    .read=my_drv_read,
    .open=NULL,
    .release=NULL,
};


/* Wenn Modul in den Kernel geladen wird */
static int __init mod_init (void)
{
    printk("init wird ausgef?hrt\n");

    if(alloc_chrdev_region(&myDevNumber,0,1,DEV_NAME)<0)
        return EIO;

    myCdev=cdev_alloc();

    if(myCdev==NULL)
        goto free_device_number;

    myCdev->owner=THIS_MODULE;
    myCdev->ops=&fops;

    if(cdev_add(myCdev,myDevNumber,1))
        goto free_cdev;

    myClass=class_create(THIS_MODULE,DEV_NAME);
    if(IS_ERR(myClass))
    {
        pr_err("Keine Anmeldung beim Udev\n");
        goto free_cdev;
    }

    myDevice=device_create(myClass,NULL,myDevNumber,NULL,"%s",DEV_NAME);

    if(IS_ERR(myDevice))
    {
        pr_err("create_device failed\n");
        goto free_class;
    }

    // gpio setup
    if(init_gpio() != 0){
        printk("Error init gpio!\n");
        goto free_class;
    };
    
    // timer setup
	timer_setup(&mytimer,my_callback,0);
	mytimer.expires=jiffies+1*HZ;
	add_timer(&mytimer);
	printk("Timer wurde gestartet, der Wert für HZ ist %d\n",HZ);

    return 0;

free_class:
    class_destroy(myClass);

free_cdev:
    kobject_put(&myCdev->kobj);

free_device_number:
    unregister_chrdev_region(myDevNumber,1);
    return -EIO;
}


/* If module is removed from kernel */
static void __exit mod_exit(void)
{
    printk("__exit wird ausgef?hrt\n");
    clean_up_gpio();
    device_destroy(myClass,myDevNumber);
    class_destroy(myClass);
    cdev_del(myCdev);
    unregister_chrdev_region(myDevNumber,1);

	if(timer_pending(&mytimer))
	printk("Timer noch aktiv\n");
	if(del_timer_sync(&mytimer))
		printk("Timer war aktiv, habe beendet!\n");
	else
		printk("War alles ok, reg dich nicht auf!\n");

    return;
}


module_init( mod_init );
module_exit( mod_exit );


/* Metainformation */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Klaas Pelzer");
MODULE_DESCRIPTION("Beispiel BK110-2" );
