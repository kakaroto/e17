EDJE_CC = edje_cc
EDJE_FLAGS = # -v

THEMES = blingbling.edj

.PHONY: all install clean

all: $(THEMES) 

blingbling.edj: blingbling.edc *.png *.ttf
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

install: all
	for f in $(THEMES); do \
		rm -f ~/.e/e/themes/$$f; \
		cp $$f ~/.e/e/themes/$$f; \
	done

clean:
	rm -f $(THEMES)
