CC = gcc
CFLAGS = -Wall -Wextra

TARGET = matrix

all: $(TARGET)

$(TARGET): matrix.c
	$(CC) $(CFLAGS) -o $(TARGET) matrix.c -lrt

clean:
	rm -f $(TARGET)
