#include <linux/module.h> /* for modules */
#include <linux/fs.h> /* file_operations */
#include <linux/uaccess.h> /* copy_(to,from)_user */
#include <linux/init.h> /* module_init, module_exit */
#include <linux/slab.h> /* kmalloc */
#include <linux/cdev.h> /* cdev utilities */
#define DRIVER_NAME "tuxdrv"
#define BUFFER_SIZE 100

static dev_t major_num = 700;
static dev_t minor_num = 0;
struct cdev my_cdev;
static char *buf; 
static struct device *ptr = NULL;
static int tux_init(void);
static void tux_exit(void);
//static void dev_open(void);
//static void dev_read(void);
//static void dev_write(void);
//static void dev_close(void);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Fang Zhu & Ziyuan Guan"); 
MODULE_DESCRIPTION("WRITING A CHARACTER SPECIAL DEVICE DRIVER");
MODULE_VERSION("0.1"); 
static struct file_operations fops =
{
   //.open = dev_open,
   //.read = dev_read,
   //.write = dev_write,
   //.close = dev_close,
};
static dev_t dev_num=0;
static int tux_init(void)  //private ?
{  
    buf = kmalloc(BUFFER_SIZE, GFP_KERNEL); //kfree
    dev_num = MKDEV(major_num, minor_num); 
    register_chrdev_region(dev_num, 2, "tux");
 
    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev_num, 2);

    printk(KERN_INFO "Loading...\n");
    return 1;  
}  

static void tux_exit(void)  //private ?
{  
    kfree(&buf);
    unregister_chrdev_region(dev_num, 2);
    cdev_del(&my_cdev);
    printk(KERN_INFO "Unloading...\n");  
    
}  


module_init(tux_init);  
module_exit(tux_exit);  
