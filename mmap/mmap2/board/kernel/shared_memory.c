#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include "shared_memory.h"

char *shared_memory_1; // app1 <-> dev1
char *shared_memory_2; // app2 <-> dev2/
char *shared_memory_3; // dev1 <-> dev2

EXPORT_SYMBOL(shared_memory_1);
EXPORT_SYMBOL(shared_memory_2);
EXPORT_SYMBOL(shared_memory_3);

static int __init shared_memory_init(void)
{
  // Allocate and initialize resources
  shared_memory_1 = kmalloc(MEMORY_SIZE, GFP_KERNEL);
  if (!shared_memory_1)
  {
    pr_err("Failed to allocate memory1\n");
    printk(KERN_ERR "Failed to allocate shared memory for Dev1\n");
    return -ENOMEM;
  }
  memset(shared_memory_1, 0, MEMORY_SIZE);

  shared_memory_2 = kmalloc(MEMORY_SIZE, GFP_KERNEL);
  if (!shared_memory_2)
  {
    pr_err("Failed to allocate memory2\n");
    printk(KERN_ERR "Failed to allocate shared memory for Dev2\n");
    kfree(shared_memory_1);
    return -ENOMEM;
  }
  memset(shared_memory_2, 0, MEMORY_SIZE);

  shared_memory_3 = kmalloc(MEMORY_SIZE, GFP_KERNEL);
  if (!shared_memory_3)
  {
    pr_err("Failed to allocate memory3\n");
    printk(KERN_ERR "Failed to allocate shared memory for Dev3\n");
    kfree(shared_memory_1);
    kfree(shared_memory_2);
    return -ENOMEM;
  }
  memset(shared_memory_3, 0, MEMORY_SIZE);

  printk(KERN_INFO "Shared memory initialized\n");

  return 0;
}

static void __exit shared_memory_exit(void)
{
  kfree(shared_memory_1);
  kfree(shared_memory_2);
  kfree(shared_memory_3);
  pr_info(KERN_INFO "Shared memory exited\n");
}

module_init(shared_memory_init);
module_exit(shared_memory_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Shared Memory Module");
