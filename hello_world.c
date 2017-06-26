#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/string.h>

char gHelloWorldString[1024];
int size;
static int dev_open(struct inode *inode, struct file *filep)
{
	printk("Device opened\n");
	return 0;
}
static int dev_release(struct inode *inode, struct file *filep)
{
	printk("Device closed\n");
	return 0;
}
static ssize_t dev_write(struct file *filep, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk("Writing to device...\n");
	if (count > 1024)
		return -1;
	if (*f_pos + count > 1024)
		*f_pos = 0;
	copy_from_user(gHelloWorldString + *f_pos, buf, count);
	return count;
}
static ssize_t dev_read(struct file *filep, char __user *buf, size_t count, loff_t *f_pos)
{
	ssize_t msgSize = sizeof(gHelloWorldString);
	if (*f_pos >= msgSize)
		return 0;
	if (*f_pos + count > msgSize)
		count = msgSize - *f_pos;
	if (copy_to_user(buf, gHelloWorldString + *f_pos, count) != 0)
		return -EFAULT;
	*f_pos += count;
	return count;
}

static struct file_operations mydev_fops = 
{
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};

static int minit(void)
{
	int result;	
	if ((result == register_chrdev(240, "thinhnguyen", &mydev_fops)) < 0)
	{
		printk(KERN_INFO "Module cannot started...\n");
		return -1;
	}
	printk(KERN_INFO "Module Started...\n");
	return 0;
}
static void mexit(void)
{
	unregister_chrdev(240, "thinhnguyen");
	printk(KERN_INFO "Module Ended...\n");
}

module_init(minit);
module_exit(mexit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thinh Nguyen");
MODULE_DESCRIPTION("Test Driver Module");
