#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/mutex.h>

#define DEVICE_NAME "dev1"
#define BUF_SIZE 4096
#define MINOR_BASE 1
#define IOCTL_MEMCPY 7

extern char *shared_memory_1;
extern char *shared_memory_3;

static DEFINE_MUTEX(mmap_mutex); // 뮤텍스

static dev_t dev1_dev;
static struct cdev dev1_cdev;
static struct class *dev1_class;

static int dev1_open(struct inode *inode, struct file *file);
static int dev1_release(struct inode *inode, struct file *file);
static int dev1_mmap(struct file *filp, struct vm_area_struct *vma);
long dev1_ioctl(struct file *filp, unsigned int cmd, unsigned long data);

static const struct file_operations dev1_fops = {
    .owner = THIS_MODULE,
    .open = dev1_open,
    .release = dev1_release,
    .mmap = dev1_mmap,
    .unlocked_ioctl = dev1_ioctl,
};

static int dev1_open(struct inode *inode, struct file *file)
{
  return 0;
}

static int dev1_release(struct inode *inode, struct file *file)
{
  return 0;
}

static int dev1_mmap(struct file *filp, struct vm_area_struct *vma)
{
  mutex_lock(&mmap_mutex); // 뮤텍스 락

  // Remap physical memory to user space
  // 그래서 가상 메모리 주소를 물리 메모리 주소로 변환
  unsigned long page = virt_to_phys(shared_memory_1) >> PAGE_SHIFT;

  // 물리 메모리 주소를 기반으로 사용자 공간에 메모리 매핑
  if (remap_pfn_range(vma, vma->vm_start, page, vma->vm_end - vma->vm_start, vma->vm_page_prot))
  {
    pr_err("Remapping failed!\n");
    mutex_unlock(&mmap_mutex); // 뮤텍스 언락
    return -EAGAIN;
  }

  mutex_unlock(&mmap_mutex); // 뮤텍스 언락
  return 0;
}

long dev1_ioctl(struct file *filp, unsigned int cmd, unsigned long data)
{
  switch (cmd)
  {
  case IOCTL_MEMCPY:
    mutex_lock(&mmap_mutex); // 뮤텍스 락

    printk(KERN_INFO "shared_memory_1: %s\n", shared_memory_1);
    memcpy(shared_memory_3, shared_memory_1, BUF_SIZE);
    printk(KERN_INFO "memory_1 -> memory_3\n");
    printk(KERN_INFO "shared_memory_3: %s\n", shared_memory_3);

    mutex_unlock(&mmap_mutex); // 뮤텍스 언락
    break;

  default:
    printk(KERN_INFO "[%s] unknown command!", __func__);
    break;
  }

  return 0;
}

static int __init dev1_init(void)
{
  int result;

  // Register the device
  if (alloc_chrdev_region(&dev1_dev, MINOR_BASE, 1, DEVICE_NAME))
  {
    pr_err("alloc_chrdev_region failed\n");
    goto err_return;
  }

  // Initialize and add the character device
  cdev_init(&dev1_cdev, &dev1_fops);
  result = cdev_add(&dev1_cdev, dev1_dev, 1);
  if (result < 0)
  {
    pr_err("Failed to add char device: %d\n", result);
    goto cleanup_data;
  }

  // Create /dev file
  dev1_class = class_create(THIS_MODULE, DEVICE_NAME);
  if (!dev1_class)
  {
    pr_err("class_create failed\n");
    goto cleanup_cdev;
  }

  if (!device_create(dev1_class, NULL, dev1_dev, NULL, DEVICE_NAME))
  {
    pr_err("Failed to create device file\n");
    goto cleanup_class;
  }

  printk(KERN_INFO "Successfully created device: Major = %d, Minor = %d\n", MAJOR(dev1_dev), MINOR(dev1_dev));

  pr_info("Character device driver loaded\n");
  return 0;

cleanup_class:
  class_destroy(dev1_class);

cleanup_cdev:
  cdev_del(&dev1_cdev);

cleanup_data:

cleanup_region:
  unregister_chrdev_region(dev1_dev, 1);

err_return:
  return result;
}

static void __exit dev1_exit(void)
{
  device_destroy(dev1_class, dev1_dev);
  class_destroy(dev1_class);
  cdev_del(&dev1_cdev);
  unregister_chrdev_region(dev1_dev, 1);
}

module_init(dev1_init);
module_exit(dev1_exit);

MODULE_AUTHOR("Your Name");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Character device driver (dev1) with mmap");