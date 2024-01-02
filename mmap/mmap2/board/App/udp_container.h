#ifndef UDP_CONTAINER_H
#define UDP_CONTAINER_H

// * DEFINES
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define UDP_RECEIVE_CMD 0x01
#define UDP_SEND_CMD 0x01

#define UDP_RECEIVE_BUF_MAX 1024
#define UDP_SEND_BUF_MAX 1024

typedef struct _UDP_RECEIVE
{
  u8 msg_type;
  u8 msg_data[UDP_RECEIVE_BUF_MAX];

} UDP_RECEIVE;

typedef struct _UDP_SEND
{
  u8 msg_type;
  u8 msg_data[UDP_RECEIVE_BUF_MAX];

} UDP_SEND;

void send_sock_data(int sockfd, uint8_t *pdata, uint16_t len, struct sockaddr_in sockaddr);

#endif