#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "init.h"

#define MAPPED_SIZE 4096

extern char *map1;
extern char *map2;
extern int dev1_fd;
extern int dev2_fd;
extern int receive_udp_fd;

// device open -> memory mmap
void mmap_init()
{
  int page_size;
  page_size = getpagesize();

  printf("Page size: %d\n", page_size);

  dev1_fd = open("/dev/dev1", O_RDWR); // O_RDWR O_WRONLY
  if (dev1_fd < 0)
  {
    perror("dev1_fd open");
    printf("failed opening device1: %s\n", strerror(errno));
  }

  dev2_fd = open("/dev/dev2", O_RDWR); // O_RDWR O_RDONLY
  if (dev2_fd < 0)
  {
    perror("dev2_fd open");
    printf("failed opening device2: %s\n", strerror(errno));
  }

  map1 = mmap(NULL, MAPPED_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev1_fd, 0);
  if (map1 == MAP_FAILED)
  {
    perror("Error in mmap1");
    printf("failed mmap1: %s\n", strerror(errno));
    close(dev1_fd);
  }

  map2 = mmap(NULL, MAPPED_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev2_fd, 0);
  if (map2 == MAP_FAILED)
  {
    perror("Error in mmap2");
    printf("failed mmap2: %s\n", strerror(errno));
    close(dev2_fd);
  }
}

// memory munmap -> device close
void all_clean()
{
  if (munmap(map1, MAPPED_SIZE) == -1)
  {
    perror("Error in munmap");
  }
  if (munmap(map2, MAPPED_SIZE) == -1)
  {
    perror("Error in munmap");
  }

  close(dev1_fd);
  close(dev1_fd);
  close(receive_udp_fd);
}