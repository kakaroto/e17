EDJE_CC = edje_cc
EDJE_FLAGS = # -v

b_and_w.edj: theme.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

.PHONY: all install clean

all: b_and_w.edj

install: all
	cp b_and_w.edj ~/.e/e/themes/b_and_w.edj

clean:
	rm -f b_and_w.edj
