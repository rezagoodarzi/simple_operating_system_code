CC = gcc
CFLAGS = -pthread
TARGET = Q4

all: $(TARGET)

$(TARGET): Q4.c
	$(CC) $(CFLAGS) -o $(TARGET) Q4.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)