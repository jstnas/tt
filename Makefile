TARGET = tt
CC = cc
CFLAGS += -lncurses -lm -Wall
INCS = $(wildcard *.h)

$(TARGET): $(TARGET).c $(INCS)
	$(CC) -o $(TARGET) $(TARGET).c $(CFLAGS)
