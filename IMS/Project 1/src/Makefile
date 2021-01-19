CC = g++
CFLAGS = -g -Wall -pedantic -pthread
TARGET = src

build: $(TARGET)
$(TARGET): $(TARGET).cc
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cc

run: $(TARGET)
	./$(TARGET) $(args)