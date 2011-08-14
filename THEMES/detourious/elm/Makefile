.PHONY: all install detourious-elm.edj

all: detourious-elm.edj

# XXX shouldn't be phony, but managing all dependence edc is a pain
detourious-elm.edj:
	edje_cc -id . -fd . default.edc -o detourious-elm.edj

install: detourious-elm.edj
	rm -f ~/.elementary/themes/detourious-elm.edj
	cp detourious-elm.edj ~/.elementary/themes/
