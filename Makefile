PREFIX = /usr/local
TARGET = tt
CC = cc
CFLAGS += -lncurses -lm -Wall -g
INCS = $(wildcard *.h)

$(TARGET): $(TARGET).c $(INCS)
	$(CC) -o $(TARGET) $(TARGET).c $(CFLAGS)

install: $(TARGET)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(TARGET) $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)
