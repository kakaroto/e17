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
	if test ! -d $(PREFIX)/share/iconbar; then \
	  mkdir $(PREFIX)/share/iconbar; \
	fi
	cp -pR data $(PREFIX)/share/iconbar/

set-config:
	echo "#define PKGDIR \"$(PREFIX)\"" > config.h; \
	echo "#define TRANS_BG" >> config.h
	touch src/iconbar.c
