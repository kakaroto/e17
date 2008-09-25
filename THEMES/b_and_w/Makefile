EDJE_CC = edje_cc
EDJE_FLAGS = # -v

THEMES = b_and_w.edj illume.edj
INITS = init.edj illume_init.edj

.PHONY: all install clean

all: $(THEMES) $(INITS)

b_and_w.edj: theme.edc *.png *.ttf
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

illume.edj: illume.edc theme.edc *.png *.ttf
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

%.edj: %.edc *.png *.ttf
	$(EDJE_CC) $(EDJE_FLAGS) $< $@


install: all
	for f in $(THEMES); do \
		rm -f ~/.e/e/themes/$$f; \
		cp $$f ~/.e/e/themes/$$f; \
	done
	for f in $(INITS); do \
		rm -f ~/.e/e/init/$$f; \
		cp $$f ~/.e/e/init/$$f; \
	done

clean:
	rm -f $(THEMES) $(INITS)
