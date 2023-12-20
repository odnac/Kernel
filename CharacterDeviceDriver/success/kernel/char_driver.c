#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/cdev.h>

#define MINOR_BASE 0
#define DEVICE_NAME "helloworld"

#define IOCTL_PRINT 1

static dev_t my_dev;
static struct class *my_class;
static struct cdev my_cdev;

static int size = 0;
static char *device_buf = NULL;

int device_open(struct inode *inode, struct file *filp);
int device_release(struct inode *inode, struct file *filp);
ssize_t device_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t device_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
long device_ioctl(struct file *filp, unsigned int cmd, unsigned long data);

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
    .unlocked_ioctl = device_ioctl};

int device_open(struct inode *inode, struct file *filp)
{
  printk(KERN_INFO "device open\n");
  return 0;
}

int device_release(struct inode *inode, struct file *filp)
{
  printk(KERN_INFO "device release\n");
  return 0;
}

long device_ioctl(struct file *filp, unsigned int cmd, unsigned long data)
{
  switch (cmd)
  {
  case IOCTL_PRINT:
    printk(KERN_INFO "[%s] IOCTL_PRINT called!", __func__);
    break;
  default:
    printk(KERN_INFO "[%s] unknown command!", __func__);
    break;
  }

  return 0;
}

ssize_t device_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
  int not_copied;

  if (device_buf != NULL)
    kfree(device_buf);

  if ((device_buf = kmalloc(count + 1, GFP_KERNEL)) == NULL)
    return -ENOMEM;

  not_copied = copy_from_user(device_buf, buf, count);
  printk("[%s} count = %ld, not_copied = %u\n", __func__, count, not_copied);

  size = count - not_copied;

  return count - not_copied;
}

ssize_t device_read(struct file *filp, char *buf, size_t count, loff_t *fpos)
{
  int not_copied;

  if (device_buf == NULL)
    return -1;

  if (count > size)
    count = size;

  not_copied = copy_to_user(buf, device_buf, count);

  printk("[%s] count = %ld, not_copied = %u\n", __func__, count, not_copied);

  return count - not_copied;
}

int __init device_init(void)
{
  if (alloc_chrdev_region(&my_dev, MINOR_BASE, 1, DEVICE_NAME))
  {
    printk(KERN_ALERT "[%s] alloc_chrdev_region failed\n", __func__);
    goto err_return;
  }

  cdev_init(&my_cdev, &fops);

  if (cdev_add(&my_cdev, my_dev, 1))
  {
    printk(KERN_ALERT "[%s] cdev_add failed\n", __func__);
    goto unreg_device;
  }

  if ((my_class = class_create(THIS_MODULE, DEVICE_NAME)) == NULL)
  {
    printk(KERN_ALERT "[%s] class_add failed\n", __func__);
    goto unreg_device;
  }

  if (device_create(my_class, NULL, my_dev, NULL, DEVICE_NAME) == NULL)
  {
    goto unreg_class;
  }

  printk(KERN_INFO "[%s] successfully created device: Major = %d, Minor = %d\n", __func__, MAJOR(my_dev), MINOR(my_dev));

  return 0;

unreg_class:
  class_destroy(my_class);

unreg_device:
  unregister_chrdev_region(my_dev, 1);

err_return:
  return -1;
}

void __exit device_exit(void)
{
  device_destroy(my_class, my_dev);
  class_destroy(my_class);
  cdev_del(&my_cdev);
  unregister_chrdev_region(my_dev, 1);
  if (device_buf != NULL)
    kfree(device_buf);
  printk("KERN_INFO [%s] successfully unregistered.\n", __func__);
}

module_init(device_init);
module_exit(device_exit);

MODULE_AUTHOR("my name");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("character device driver");