TARGET = tt
CC = cc
CFLAGS += -lncurses -lm
INCS = $(wildcard *.h)

$(TARGET): $(TARGET).c $(INCS)
	$(CC) -o $(TARGET) tt.c $(CFLAGS)
