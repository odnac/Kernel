#ifndef INIT_H_
#define INIT_H_

#define MAPPED_SIZE 1024

#define SEND_TO_IP "192.168.0.0"
#define SEND_PORT 4000
#define RECEIVE_PORT 5000

void mmap_init();
void socket_init();
void all_clean();

#endif // INIT_H_
