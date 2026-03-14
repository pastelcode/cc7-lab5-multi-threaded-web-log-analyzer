CC     = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = main
SRCS   = main_linux.c log_processor.c hashmap.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) -lpthread

clean:
	rm -f $(TARGET)
