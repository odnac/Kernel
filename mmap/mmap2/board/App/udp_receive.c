#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include "init.h"
#include "udp_container.h"
#include "queue.h"
#include "global.h"
#include "udp_send.h"

void *receive_Thr(void *arg)
{
  int recvlen = 0;
  int selcnt = 0;
  fd_set fdset;
  UDP_RECEIVE receive_message;
  Queue *recv_buffer = (Queue *)arg;

  printf("UDP %d 포트에서 리시브 중...\n", RECEIVE_PORT);

  while (1)
  {
    FD_ZERO(&fdset);
    FD_SET(receive_udp_fd, &fdset);

    selcnt = select(receive_udp_fd + 1, &fdset, NULL, NULL, NULL);

    if (selcnt > 0 && FD_ISSET(receive_udp_fd, &fdset))
    {
      recvlen = recvfrom(receive_udp_fd, (void *)&receive_message, sizeof(receive_message), 0, NULL, NULL);
      if (recvlen <= 0)
      {
        continue;
      }

      if (receive_message.msg_type == UDP_SEND_CMD)
      {
        pthread_mutex_lock(&mutex);
        memcpy(message_copy, receive_message.msg_data, UDP_RECEIVE_BUF_MAX);
        // printf("message_copy : %s\n", message_copy);
        pthread_cond_signal(&cond);
        queue_enqueue(recv_buffer, message_copy);
        pthread_mutex_unlock(&mutex);
      }
    }
  }
}

void *dequeue_Thr(void *arg)
{
  Queue *recv_buffer = (Queue *)arg;

  while (1)
  {
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    char *data = queue_dequeue(recv_buffer);
    // printf("data : %s\n", data);

    // app1
    if (data != NULL)
    {
      memcpy(map1, data, UDP_RECEIVE_BUF_MAX);
    }

    // memcpy1
    if (ioctl(dev1_fd, IOCTL_MEMCPY, NULL) < 0)
    {
      perror("Error in dev1_fd ioctl");
    }

    // memcpy2
    if (ioctl(dev2_fd, IOCTL_MEMCPY, NULL) < 0)
    {
      perror("Error in dev2_fd ioctl");
    }

    // app2 -> udp send
    // printf("\n");
    // printf("read memory2 : %s\n", map2);
    // printf("\n");
    udp_send();

    pthread_mutex_unlock(&mutex);
  }
}

// for UDP receive => mutex, cond, socket init
void udp_receive()
{
  pthread_t receive_udp_thread;
  pthread_t dequeue_thread;

  int return_value = 0;
  Queue *recv_buffer = queue_init();

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  return_value = pthread_create(&receive_udp_thread, 0, receive_Thr, recv_buffer);
  if (return_value != 0)
  {
    printf("receive thread_create : %s", strerror(return_value));
    return;
  }

  return_value = pthread_create(&dequeue_thread, 0, dequeue_Thr, recv_buffer);
  if (return_value != 0)
  {
    printf("dequeue_Thr create : %s", strerror(return_value));
    return;
  }

  pthread_join(receive_udp_thread, NULL);
  pthread_join(dequeue_thread, NULL);
}