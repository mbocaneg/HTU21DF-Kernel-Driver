
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/printk.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include<linux/slab.h>
#include<linux/uaccess.h> 

#define DEVICE_NAME "htu21df"
#define DEFAULT_ADDR 0x40

#define HUMIDITY_NO_HOLD 0xF5
#define TEMP_NO_HOLD 0xF3

#define HUMIDITY_HOLD 0xE5
#define TEMP_HOLD 0xE3

static dev_t dev;
static struct class *dev_class;
struct cdev cdev;

static struct htu21df {
    struct i2c_client *i2c_client;
    u8 readings[4];
};
struct htu21df *sensor;

static ssize_t htu21df_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Reading HTU21DF\n");
    u16 raw_humidity = i2c_smbus_read_word_swapped(sensor->i2c_client, HUMIDITY_HOLD) & 0x0000FFFC;
    u16 raw_temp = i2c_smbus_read_word_swapped(sensor->i2c_client, TEMP_HOLD) & 0x0000FFFC;

    sensor->readings[0] = raw_temp & 0xFF;
    sensor->readings[1] = (raw_temp >> 8) & 0xFF;

    sensor->readings[2] = raw_humidity & 0xFF;
    sensor->readings[3] = (raw_humidity >> 8) & 0xFF;

    copy_to_user(buf, sensor->readings, 4);
    return 0;
}

static ssize_t htu21df_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Writing HTU21DF\n");
    return len;
}

static int htu21df_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device File Opened...!!!\n");
        return 0;
}

static int htu21df_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device File Closed...!!!\n");
        return 0;
}

static struct file_operations fops =
{
	.owner          = THIS_MODULE,
	.read           = htu21df_read,
	.write          = htu21df_write,
    .open          = htu21df_open,
    .release        = htu21df_release,
};

static const struct i2c_device_id htu21df_id_table[] = {
      { "htu21df", 0 },
      { }
};

MODULE_DEVICE_TABLE(i2c, htu21df_id_table);

static int htu21df_probe(struct i2c_client *i2c_client, const struct i2c_device_id *id){
    printk(KERN_INFO "Probing HTU21DF\n");
    sensor = (struct htu21df *)kmalloc(sizeof(struct htu21df), GFP_KERNEL);
    i2c_client->addr = DEFAULT_ADDR;
    sensor->i2c_client = i2c_client;

    int i;
    for(i = 0; i < 4; i++){
        sensor->readings[i] = 0;
    }

    return 0;
}
static int htu21df_remove(struct i2c_client *client)
{
    kfree(sensor);
    return 0;
}

static struct i2c_driver htu21df_driver = {
    .driver = {
        .name = "htu21df",
        .owner = THIS_MODULE,
    },

    .id_table = htu21df_id_table,
    .probe = htu21df_probe,
    .remove = htu21df_remove,
};


static int __init htu21df_driver_init(void)
{

    if((alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME)) <0){
        printk(KERN_INFO "Cannot allocate major number\n");
        return -1;
    }
    printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

    cdev_init(&cdev,&fops);
    if((cdev_add(&cdev,dev,1)) < 0){
        printk(KERN_INFO "Cannot add the device to the system\n");
        unregister_chrdev_region(dev,1);
        return -1;
    }

    if((dev_class = class_create(THIS_MODULE,DEVICE_NAME)) == NULL){
        printk(KERN_INFO "Cannot create the struct class\n");
        unregister_chrdev_region(dev,1);
        return -1;
    }

    if((device_create(dev_class,NULL,dev,NULL,DEVICE_NAME)) == NULL){
        printk(KERN_INFO "Cannot create the Device 1\n");
        unregister_chrdev_region(dev,1);
        return -1;;
    }
    
    i2c_add_driver(&htu21df_driver);
    printk(KERN_INFO "Device Driver HTU21DF Init\n");

    return 0;

}

void __exit htu21df_driver_exit(void)
{
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&cdev);
    unregister_chrdev_region(dev, 1);
    i2c_del_driver(&htu21df_driver);
    printk(KERN_INFO "hTU21DF REMOVED!\n");
}

module_init(htu21df_driver_init);
module_exit(htu21df_driver_exit);

MODULE_AUTHOR("mbocaneg");
MODULE_DESCRIPTION("Driver for Htu21df Temp and Humidity Sensor");

MODULE_LICENSE("GPL");
