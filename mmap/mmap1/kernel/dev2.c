#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/mutex.h>

#define DEVICE_NAME "dev2"
#define BUF_SIZE 4096
#define MINOR_BASE 2
#define IOCTL_MEMCPY 7

extern char *shared_memory_2;
extern char *shared_memory_3;

static DEFINE_MUTEX(mmap_mutex); // 뮤텍스

static dev_t dev2_dev;
static struct cdev dev2_cdev;
static struct class *dev2_class;

static DECLARE_WAIT_QUEUE_HEAD(wq);
// static int flag = 0;

static int dev2_open(struct inode *inode, struct file *file);
static int dev2_release(struct inode *inode, struct file *file);
static int dev2_mmap(struct file *filp, struct vm_area_struct *vma);
long dev2_ioctl(struct file *filp, unsigned int cmd, unsigned long data);

static const struct file_operations dev2_fops = {
    .owner = THIS_MODULE,
    .open = dev2_open,
    .release = dev2_release,
    .mmap = dev2_mmap,
    .unlocked_ioctl = dev2_ioctl,
};

static int dev2_open(struct inode *inode, struct file *file)
{
  return 0;
}

static int dev2_release(struct inode *inode, struct file *file)
{
  return 0;
}

static int dev2_mmap(struct file *filp, struct vm_area_struct *vma)
{
  mutex_lock(&mmap_mutex); // 뮤텍스 락

  // Remap physical memory to user space
  unsigned long page = virt_to_phys(shared_memory_2) >> PAGE_SHIFT;
  if (remap_pfn_range(vma, vma->vm_start, page, vma->vm_end - vma->vm_start, vma->vm_page_prot))
  {
    pr_err("Remapping failed!\n");
    mutex_unlock(&mmap_mutex); // 뮤텍스 언락
    return -EAGAIN;
  }

  mutex_unlock(&mmap_mutex); // 뮤텍스 언락
  return 0;
}

long dev2_ioctl(struct file *filp, unsigned int cmd, unsigned long data)
{
  switch (cmd)
  {
  case IOCTL_MEMCPY:
    mutex_lock(&mmap_mutex); // 뮤텍스 락

    printk(KERN_INFO "shared_memory_3: %s\n", shared_memory_3);
    memcpy(shared_memory_2, shared_memory_3, BUF_SIZE);
    printk(KERN_INFO "memory_3 -> memory_2\n");
    printk(KERN_INFO "memcpy shared_memory_2: %s\n", shared_memory_2);

    mutex_unlock(&mmap_mutex); // 뮤텍스 언락
    break;
  default:
    printk(KERN_INFO "[%s] unknown command!", __func__);
    break;
  }

  return 0;
}

static int __init dev2_init(void)
{
  int result;

  // Register the device
  if (alloc_chrdev_region(&dev2_dev, MINOR_BASE, 1, DEVICE_NAME))
  {
    pr_err("alloc_chrdev_region failed\n");
    goto err_return;
  }

  // Initialize and add the character device
  cdev_init(&dev2_cdev, &dev2_fops);
  result = cdev_add(&dev2_cdev, dev2_dev, 1);
  if (result < 0)
  {
    pr_err("Failed to add char device: %d\n", result);
    goto cleanup_data;
  }

  // Create /dev file
  dev2_class = class_create(THIS_MODULE, DEVICE_NAME);
  if (!dev2_class)
  {
    pr_err("class_create failed\n");
    goto cleanup_cdev;
  }

  if (!device_create(dev2_class, NULL, dev2_dev, NULL, DEVICE_NAME))
  {
    pr_err("Failed to create device file\n");
    goto cleanup_class;
  }

  printk(KERN_INFO "Successfully created device: Major = %d, Minor = %d\n", MAJOR(dev2_dev), MINOR(dev2_dev));

  pr_info("Character device driver loaded\n");
  return 0;

cleanup_class:
  class_destroy(dev2_class);

cleanup_cdev:
  cdev_del(&dev2_cdev);

cleanup_data:

cleanup_region:
  unregister_chrdev_region(dev2_dev, 1);

err_return:
  return result;
}

static void __exit dev2_exit(void)
{
  device_destroy(dev2_class, dev2_dev);
  class_destroy(dev2_class);
  cdev_del(&dev2_cdev);
  unregister_chrdev_region(dev2_dev, 1);
  // pr_info("Character device driver unloaded\n");
}

module_init(dev2_init);
module_exit(dev2_exit);

MODULE_AUTHOR("Your Name");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Character device driver (dev2) with mmap");