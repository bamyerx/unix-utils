ROOT=../..

include $(ROOT)/config.mk

TARGET=$(ROOT)/build/echo

.PHONY: all clean

all: $(TARGET)

$(TARGET): echo.c | $(ROOT)/build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

../../build:
	mkdir -p $@

clean:
	rm -f $(TARGET)
