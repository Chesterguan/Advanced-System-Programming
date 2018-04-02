#include <linux/module.h> /* for modules */
#include <linux/fs.h> /* file_operations */
#include <linux/uaccess.h> /* copy_(to,from)_user */
#include <linux/init.h> /* module_init, module_exit */
#include <linux/slab.h> /* kmalloc */
#include <linux/cdev.h> /* cdev utilities */
#define DRIVER_NAME "mycdrv"
#define BUFFER_SIZE 100
#define ramdisk_size(size_t)(16*PAGE_SIZE)

static unsigned int count =1;
static int major=700,minor=0;
static struct asp_mycdev {
	struct cdev dev;
	char *ramdisk;
	struct semaphore sem;
	int devNo;
};

static const struct file_operations mycdrv_fops={
	.owner = THIS_MODULE,
	.read = mycdrv_read,
	.write = mycdrv_write,
	.open = mycdrv_open,
	.release = mycdrv_release,
};
static int _int my_init(void){
	
