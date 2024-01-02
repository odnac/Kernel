CC = gcc
CFLAGS = -Wall -Wextra

TARGET = udp_receive

all: $(TARGET)

$(TARGET): udp_receive.c
	$(CC) $(CFLAGS) -o $(TARGET) udp_receive.c -static

clean:
	rm -f $(TARGET)
