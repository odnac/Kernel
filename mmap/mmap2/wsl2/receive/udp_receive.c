#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "udp_receive.h"

#define SERVER_PORT 4000
#define INTERVAL_SEC 1

UDP_RECEIVE udp_receive_message;

void format_Number(long num, char *formattedNum);

void udp_receive()
{
  struct sockaddr_in receiver_addr;

  int receiver_socket = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&receiver_addr, 0, sizeof(receiver_addr));
  receiver_addr.sin_family = AF_INET;
  receiver_addr.sin_addr.s_addr = INADDR_ANY;
  receiver_addr.sin_port = htons(SERVER_PORT);
  if (bind(receiver_socket, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)))
  {
    printf("bind Error\n");
    exit(1);
  }

  printf("UDP %d 포트에서 대기 중...\n", SERVER_PORT);

  time_t start_time = time(NULL);
  long total_bytes_received = 0;

  while (1)
  {
    ssize_t recv_len = recvfrom(receiver_socket, (void *)&udp_receive_message, UDP_RECEIVE_BUF_MAX, 0, NULL, NULL);
    if (recv_len <= 0)
    {
      continue;
    }

    if (udp_receive_message.msg_type == UDP_RECEIVE_CMD)
    {
      total_bytes_received += recv_len;

      // 1초마다 받은 데이터양 출력
      time_t current_time = time(NULL);
      if (current_time - start_time >= INTERVAL_SEC)
      {
        printf("데이터 수신: %s\n", udp_receive_message.msg_data);
        char format_total_bytes_received[20];
        format_Number(total_bytes_received, format_total_bytes_received);
        printf("받은 데이터 양: %s \n\n", format_total_bytes_received);

        // 다음 1초를 위해 카운터 초기화
        start_time = current_time;
        total_bytes_received = 0;
      }
    }
  }

  close(receiver_socket);
}

int main()
{
  udp_receive();
  return 0;
}

void format_Number(long num, char *formattedNum)
{
  int len = snprintf(NULL, 0, "%ld", num);
  char *str = malloc(len + 1);
  snprintf(str, len + 1, "%ld", num);

  int j = 0;
  for (int i = 0; i < len + 1; i++)
  {
    formattedNum[j++] = str[i];
    if ((len - i) % 3 == 1 && (len - i) > 1)
    {
      formattedNum[j++] = ',';
    }
  }
  formattedNum[j] = '\0';
  free(str);
}
