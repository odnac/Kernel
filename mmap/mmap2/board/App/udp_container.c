#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// sendto
void send_sock_data(int sockfd, uint8_t *pdata, uint16_t len, struct sockaddr_in sockaddr)
{
  if (sendto(sockfd, pdata, len, 0, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1)
  {
    perror("sendto");
  }
}