CC = gcc
CFLAGS = -pthread
TARGET = Q3

all: $(TARGET)

$(TARGET): Q3.c
	$(CC) $(CFLAGS) -o $(TARGET) Q3.c

run: $(TARGET)
	./$(TARGET) <source_file_path> <destination_file_path>

clean:
	rm -f $(TARGET)