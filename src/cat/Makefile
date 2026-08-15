ROOT=../..

include $(ROOT)/config.mk

TARGET=$(ROOT)/build/cat

.PHONY: all clean

all: $(TARGET)

$(TARGET): cat.c | $(ROOT)/build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

../../build:
	mkdir -p $@

clean:
	rm -f $(TARGET)
