TARGET = tt
CC = cc
CFLAGS += -lncurses
INCS = config.h menu.h node.h tbackend.h twindow.h vector2.h window.h words.h

$(TARGET): tt.c $(INCS)
	$(CC) -o $(TARGET) tt.c $(CFLAGS)
