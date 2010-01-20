EDJE_FLAGS = -v -id img/ -id img/icons -fd fonts/

themedir = $(prefix)/share/eyesight/themes/docker
theme_DATA = docker.edj

docker.edj: Makefile
	edje_cc $(EDJE_FLAGS) main.edc -o docker.edj
	
clean:
	rm -f *.edj

EXTRA_DIST = $(wildcard *.edc) $(wildcard fonts/*.ttf)
