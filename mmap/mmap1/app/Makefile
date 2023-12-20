
CC = gcc

TARGET = app_test

OBJS	 = main.c
OBJS	+= global.c
OBJS	+= init.c
OBJS	+= communication.c

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

clean:
	rm -f $(OBJS) $(TARGET)