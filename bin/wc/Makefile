ROOT=../..

include $(ROOT)/config.mk

TARGET=$(ROOT)/build/wc

.PHONY: all clean

all: $(TARGET)

$(TARGET): wc.c | $(ROOT)/build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

../../build:
	mkdir -p $@

clean:
	rm -f $(TARGET)
