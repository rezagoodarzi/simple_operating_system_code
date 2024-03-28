CC = gcc
CFLAGS = -Wall -Wextra

TARGET = shell

all: $(TARGET)

$(TARGET): shell.c
	$(CC) $(CFLAGS) -o $(TARGET) shell.c

clean:
	rm -f $(TARGET)
