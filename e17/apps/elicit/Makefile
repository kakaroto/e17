PREFIX=/usr/local

all: src

clean: src-clean

install: install-src install-data

src: set-config 
	cd src; make; cd ..

src-clean:
	cd src; make clean; cd ..
        
install-src:
	cd src; make install PREFIX=$(PREFIX); cd ..

install-data:
	if test ! -d $(PREFIX)/share/elicit; then \
	  mkdir $(PREFIX)/share/elicit; \
	fi
	cp -pR data $(PREFIX)/share/elicit

set-config:
	echo "#define PKGDIR \"$(PREFIX)\"" > config.h; \
        touch src/elicit.c
