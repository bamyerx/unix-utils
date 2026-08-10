PROGRAMS= echo

.PHONY: all clean $(PROGRAMS)

all: $(PROGRAMS)

$(PROGRAMS):
	$(MAKE) -C bin/$@

clean:
	for dir in $(PROGRAMS); do \
		$(MAKE) -C bin/$$dir clean; \
	done
