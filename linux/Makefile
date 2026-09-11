CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -std=c11
GTK_CFLAGS := $(shell pkg-config --cflags gtk4)
GTK_LIBS := $(shell pkg-config --libs gtk4)

TARGET := progman3
OBJS := progman3.o grp_parser.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(GTK_LIBS)

progman3.o: progman3.c grp_parser.h
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -c $< -o $@

grp_parser.o: grp_parser.c grp_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
