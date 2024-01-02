CC = gcc

TARGET = app_test 

OBJS   = main.c
OBJS	+= global.c
OBJS	+= init.c
OBJS	+= udp_container.c
OBJS	+= udp_receive.c
OBJS	+= queue.c
OBJS	+= udp_send.c

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

clean:
	rm -f $(OBJS) $(TARGET)