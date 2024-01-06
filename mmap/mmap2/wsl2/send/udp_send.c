#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include "udp_send.h"

#define BOARD_IP "192.168.0.253"
#define BOARD_PORT 54321

#define TARGET_BANDWIDTH 1000000 // 1 Mbps
// #define TARGET_BANDWIDTH 5000000 // 5 Mbps
// #define TARGET_BANDWIDTH 10000000 // 10 Mbps
// #define TARGET_BANDWIDTH 20000000 // 20 Mbps

struct sockaddr_in sender_addr;
int sender_socket;

UDP_SEND udp_send_data;
long total_bytes_sended = 0;

void signal_handler(int sig)
{
  if (sig == SIGALRM)
  {
    printf("보낸 데이터 양 : %ld, %0.2f mbps\n", total_bytes_sended, ((double)((double)total_bytes_sended * 8) / 1000000));
    total_bytes_sended = 0;
    alarm(1);
  }
}

void udp_send()
{
  long bytes_per_second = TARGET_BANDWIDTH / 8;

  signal(SIGALRM, signal_handler); // Set up the signal handler
  alarm(1);                        // Schedule the first signal in 1 second

  udp_send_data.msg_type = UDP_SEND_CMD;

  struct timespec start, end;

  int send_count = bytes_per_second / sizeof(UDP_SEND);
  for (int i = 0; i < send_count; i++)
  {
    clock_gettime(CLOCK_REALTIME, &start);
    ssize_t bytes_sent = sendto(sender_socket, &udp_send_data, sizeof(udp_send_data), 0, (struct sockaddr *)&sender_addr, sizeof(sender_addr));
    if (bytes_sent > 0)
    {
      total_bytes_sended += bytes_sent;
    }
    clock_gettime(CLOCK_REALTIME, &end);

    // double time_between_packets = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
  }
}

int main()
{
  sender_socket = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&sender_addr, 0, sizeof(sender_addr));
  sender_addr.sin_family = AF_INET;
  sender_addr.sin_addr.s_addr = inet_addr(BOARD_IP);
  sender_addr.sin_port = htons(BOARD_PORT);
  if (sender_socket < 0)
  {
    perror("socket");
    exit(1);
  }
  while (1)
  {
    udp_send();
    sleep(1);
  }

  close(sender_socket);
  return 0;
}
