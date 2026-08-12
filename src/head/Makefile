ROOT=../..

include $(ROOT)/config.mk

TARGET=$(ROOT)/build/head

.PHONY: all clean

all: $(TARGET)

$(TARGET): head.c | $(ROOT)/build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

../../build:
	mkdir -p $@

clean:
	rm -f $(TARGET)
