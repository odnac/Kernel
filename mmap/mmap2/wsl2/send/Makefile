CC = gcc
CFLAGS = -Wall -Wextra

TARGET = udp_send

all: $(TARGET)

$(TARGET): udp_send.c
	$(CC) $(CFLAGS) -o $(TARGET) udp_send.c

clean:
	rm -f $(TARGET)
