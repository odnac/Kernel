#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <pthread.h>

#define MSG_SIZE 1024
#define IOCTL_MEMCPY 7

extern char *map1;
extern char *map2;
extern int dev1_fd;
extern int dev2_fd;
extern int receive_udp_fd;
extern int send_udp_fd;
extern char message_copy[MSG_SIZE];
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern struct sockaddr_in send_udp_Addr;

#endif // GLOBAL_H_