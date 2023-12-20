obj-m := shared_memory.o
obj-m += dev1.o
obj-m += dev2.o

KDIR := [YOUR KERNEL SOURCE LOCATION]
PWD := $(shell pwd)

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
