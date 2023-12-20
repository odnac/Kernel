#include <stdio.h>

extern char *map1;
extern char *map2;
extern int dev1_fd;
extern int dev2_fd;

#define IOCTL_MEMCPY 7
#define MAPPED_SIZE 4096
#define LEN 500

// write to memory1
void app1()
{
  char *str = "hello app2! this is app1!";

  strncpy(map1, str, strlen(str));
  printf("Write to memory1 : %s\n", map1);

  // // 동기화
  // msync(map1, MAPPED_SIZE, MS_SYNC);
  // {
  //   perror("Error in msync");
  //   printf("errno: %d\n", errno);
  // }
}

// memory1 -> memory3
void memcpy1()
{
  if (ioctl(dev1_fd, IOCTL_MEMCPY, NULL) < 0)
  {
    perror("Error in ioctl");
  }
}

// memory3 -> memory2
void memcpy2()
{
  if (ioctl(dev2_fd, IOCTL_MEMCPY, NULL) < 0)
  {
    perror("Error in ioctl");
  }
}

// read memory2
void app2()
{
  printf("\n");
  printf("read memory2 : %s\n", map2);
  printf("\n");
}
