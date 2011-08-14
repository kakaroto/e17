.PHONY: all install detourious.edj

all: detourious.edj

# XXX shouldn't be phony, but managing all dependence edc is a pain
detourious.edj:
	edje_cc -id . -fd . main.edc -o detourious.edj

install: detourious.edj
	rm -f ~/.e/e/themes/detourious.edj
	cp detourious.edj ~/.e/e/themes
