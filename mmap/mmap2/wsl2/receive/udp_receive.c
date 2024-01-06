#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>
#include "udp_receive.h"

#define SERVER_PORT 12345

UDP_RECEIVE udp_receive_message;
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

int main()
{
  struct sockaddr_in receiver_addr;
  int receiver_socket;
  ssize_t recv_len;
  uint32_t test_cnt = 0;

  receiver_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (receiver_socket < 0)
  {
    perror("socket failed");
    return EXIT_FAILURE;
  }

  memset(&receiver_addr, 0, sizeof(receiver_addr));
  receiver_addr.sin_family = AF_INET;
  receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  receiver_addr.sin_port = htons(SERVER_PORT);

  if (bind(receiver_socket, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0)
  {
    perror("bind failed");
    return EXIT_FAILURE;
  }

  printf("UDP %d 포트에서 대기 중...\n", SERVER_PORT);

  signal(SIGALRM, signal_handler);
  alarm(1);

  while (1)
  {
    recv_len = recvfrom(receiver_socket, (void *)&udp_receive_message, sizeof(udp_receive_message), 0, NULL, NULL);
    if (recv_len > 0)
    {
      total_bytes_received += recv_len;
    }
  }

  close(receiver_socket);
}
