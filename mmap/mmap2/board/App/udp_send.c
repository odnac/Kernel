#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
// #include <arpa/inet.h>
// #include <sys/socket.h>
#include <sys/time.h>
#include "udp_container.h"
#include "global.h"
#include "init.h"

void udp_send()
{
  UDP_SEND send_message;
  send_message.msg_type = UDP_SEND_CMD;
  memcpy(send_message.msg_data, map2, UDP_SEND_BUF_MAX);

  // printf("보냄 : %s\n", send_message.msg_data);

  // printf("%s:%d로 데이터 전송 중...\n", SEND_TO_IP, SEND_PORT);

  sendto(send_udp_fd, &send_message, sizeof(send_message), 0, (struct sockaddr *)&send_udp_Addr, sizeof(struct sockaddr_in));
}