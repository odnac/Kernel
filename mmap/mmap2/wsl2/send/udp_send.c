#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "udp_send.h"

#define BOARD_IP "192.168.0.0"
#define BOARD_PORT 5000
#define TARGET_BANDWIDTH 10000000 // 10 Mbps

UDP_SEND udp_send_message;

void format_Number(long num, char *formattedNum);

void udp_send()
{
  long total_bytes_sent = 0;
  struct timeval current_time;
  char ex_message[UDP_SEND_BUF_MAX];
  struct sockaddr_in sender_addr;
  int sender_socket = socket(AF_INET, SOCK_DGRAM, 0);
  char format_total_bytes_sent[20];

  memset(&sender_addr, 0, sizeof(sender_addr));
  sender_addr.sin_family = AF_INET;
  sender_addr.sin_addr.s_addr = inet_addr(BOARD_IP);
  sender_addr.sin_port = htons(BOARD_PORT);
  if (sender_socket < 0)
  {
    perror("socket");
    exit(1);
  }

  udp_send_message.msg_type = UDP_SEND_CMD;

  printf("%s:%d로 데이터 전송 중...\n", BOARD_IP, BOARD_PORT);

  while (1)
  {
    // 현재 시간 기록
    gettimeofday(&current_time, NULL);
    total_bytes_sent = 0;

    long seconds = current_time.tv_sec;
    long milliseconds = current_time.tv_usec;

    int minutes = (seconds % 3600) / 60;
    int seconds_remain = seconds % 60;

    sprintf(ex_message, "Time: %02d분 %02d초.%04ld", minutes, seconds_remain, milliseconds);

    memcpy(udp_send_message.msg_data, ex_message, strlen(ex_message));

    // 1초에 TARGET_BANDWIDTH 바이트를 전송하도록 대기
    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = (1.0 / TARGET_BANDWIDTH) * 1000000000;

    time_t start_time = time(NULL);
    while (time(NULL) - start_time < 1)
    {
      ssize_t bytes_sent = sendto(sender_socket, &udp_send_message, sizeof(udp_send_message), 0, (struct sockaddr *)&sender_addr, sizeof(sender_addr));
      if (bytes_sent > 0)
      {
        total_bytes_sent += bytes_sent;
      }

      // 대역폭에 맞게 대기
      nanosleep(&req, NULL);
    }

    format_Number(total_bytes_sent, format_total_bytes_sent);

    // printf("데이터 송신: %s\n", ex_message);
    printf("보낸 데이터 양 : %s\n\n", format_total_bytes_sent);
  }

  // 소켓 닫기
  close(sender_socket);
}

int main()
{
  udp_send();
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