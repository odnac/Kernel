
typedef unsigned char u8;
typedef unsigned short u16;

#define UDP_RECEIVE_BUF_MAX 1024

#define UDP_RECEIVE_CMD 0x01

typedef struct _UDP_RECEIVE
{
  u16 sequence_number;
  u8 msg_type;
  u8 msg_data[UDP_RECEIVE_BUF_MAX];

} UDP_RECEIVE;