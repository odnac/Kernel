
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

static char *buffer = NULL;

int virtual_device_open(struct inode *inode, struct file *filp)
{
  printk(KERN_ALERT "virtual_device open function called\n");
  return 0;
}

int virtual_device_release(struct inode *inode, struct file *filp)
{
  printk(KERN_ALERT "virtual device release function called\n");
  return 0;
}

ssize_t virtual_device_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
  printk(KERN_ALERT "virtual_device write function called\n");
  strcpy(buffer, buf);
  return count;
}

// 예제 코드 에러 떠서 코드 수정함
// ssize_t virtual_device_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
//     printk(KERN_ALERT "virtual_device read function called\n");
//     copy_to_user(buf, buffer, 1024);
//     return count;
// }

ssize_t virtual_device_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
  ssize_t bytes_read;

  printk(KERN_ALERT "virtual_device read function called\n");

  if (copy_to_user(buf, buffer, 1024))
  {
    printk(KERN_ALERT "Failed to copy data to user space\n");
    bytes_read = -EFAULT; // Set an appropriate error code
  }
  else
  {
    bytes_read = count; // Indicate successful read
  }

  return bytes_read;
}

static struct file_operations vd_fops = {
    .read = virtual_device_read,
    .write = virtual_device_write,
    .open = virtual_device_open,
    .release = virtual_device_release};

int __init virtual_device_init(void)
{
  if (register_chrdev(250, "virtual_device", &vd_fops) < 0)
    printk(KERN_ALERT "driver init failed\n");
  else
    printk(KERN_ALERT "driver init successful\n");
  buffer = (char *)kmalloc(1024, GFP_KERNEL);
  if (buffer != NULL)
    memset(buffer, 0, 1024);
  return 0;
}

void __exit virtual_device_exit(void)
{
  unregister_chrdev(250, "virtual_device");
  printk(KERN_ALERT "driver cleanup successful\n");
  kfree(buffer);
}

module_init(virtual_device_init);
module_exit(virtual_device_exit);
MODULE_LICENSE("GPL");