#include <stdio.h>
#include <netinet/in.h>
#include "global.h"

char *map1 = NULL;
char *map2 = NULL;
int dev1_fd = 0;
int dev2_fd = 0;
int receive_udp_fd = -1;
int send_udp_fd = -1;
char message_copy[MSG_SIZE];
pthread_mutex_t mutex;
pthread_cond_t cond;
struct sockaddr_in send_udp_Addr;