PROGRAMS= echo wc head

.PHONY: all clean $(PROGRAMS)

all: $(PROGRAMS)

$(PROGRAMS):
	$(MAKE) -C src/$@

clean:
	for dir in $(PROGRAMS); do \
		$(MAKE) -C src/$$dir clean; \
	done
