#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "init.h"
#include "udp_container.h"
#include "queue.h"
#include "global.h"
#include "udp_send.h"

long total_bytes_received = 0;

void signal_handler(int sig)
{
  if (sig == SIGALRM)
  {
    printf("받은 데이터 양 : %ld, %0.2f mbps\n", total_bytes_received, ((double)((double)total_bytes_received * 8) / 1000000));
    total_bytes_received = 0;
    alarm(1);
  }
}

void *receive_Thr(void *arg)
{
  fd_set fdset;
  ssize_t recvlen = 0;
  UDP_RECEIVE receive_message;

  printf("UDP %d 포트에서 리시브 중...\n", RECEIVE_PORT);

  signal(SIGALRM, signal_handler);
  alarm(1);

  while (1)
  {
    recvlen = recvfrom(receive_udp_fd, (void *)&receive_message, sizeof(receive_message), 0, NULL, NULL);
    if (recvlen <= 0)
    {
      continue;
    }

    if (receive_message.msg_type == UDP_SEND_CMD)
    {
      total_bytes_received += recvlen;

      if (receive_message.msg_data != NULL)
      {
        memcpy(map1, receive_message.msg_data, UDP_RECEIVE_BUF_MAX);
      }

      if (ioctl(dev1_fd, IOCTL_MEMCPY, NULL) < 0)
      {
        perror("Error in dev1_fd ioctl");
      }

      if (ioctl(dev2_fd, IOCTL_MEMCPY, NULL) < 0)
      {
        perror("Error in dev2_fd ioctl");
      }

      udp_send();
    }
  }
}

// for UDP receive => mutex, cond, socket init
void udp_receive()
{
  pthread_t receive_udp_thread;

  int return_value = 0;
  Queue *recv_buffer = queue_init();

  return_value = pthread_create(&receive_udp_thread, 0, receive_Thr, recv_buffer);
  if (return_value != 0)
  {
    printf("receive thread_create : %s", strerror(return_value));
    return;
  }

  pthread_join(receive_udp_thread, NULL);
}