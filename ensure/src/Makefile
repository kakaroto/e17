PKGS=elementary

CFLAGS+=`pkg-config --cflags ${PKGS}` 
LDFLAGS+=`pkg-config --libs ${PKGS}` -rdynamic -ldl

OBJECTS=			\
		display.o	\
		enasn.o		\
		enedj.o		\
		ensure.o	\
		enobj.o		\
		parser.o

ASSURANCES=					\
		assurances/badclipsmart.asn	\
		assurances/clippedsmart.asn	\
		assurances/edjeerr.asn		\
		assurances/emptytext.asn	\
		assurances/imageerr.asn		\
		assurances/largetext.asn	\
		assurances/negativesize.asn	\
		assurances/offscreen.asn	\
		assurances/outofclip.asn	\
		assurances/outofsmart.asn	\
		assurances/premul.asn		\
		assurances/smalltext.asn	\
		assurances/trailingspace.asn	\
		assurances/zeroalpha.asn	\
		assurances/zerosize.asn	

TARGET=ensure
LIBTARGET=libensure.so.0

all: ${TARGET} ${LIBTARGET} ${ASSURANCES}

${TARGET}: ${OBJECTS}

%.asn: %.c
	${CC} -I.. -Os -fPIC -o $@ -shared -Wl,-soname,$@ $< ${CFLAGS} ${LDFLAGS}

libensure.so.0: libensure.c
	${CC} -fPIC -Os -Wall -shared -Wl,-soname,${LIBTARGET} -o ${LIBTARGET} \
		`pkg-config --cflags --libs ecore evas`	\
		libensure.c

scan: clean
	scan-build make -j 4

.PHONY: tags
tags:
	ctags -R .
	cscope -R -b

clean:
	rm -rf ${TARGET} *.o core ${ASSURANCES}

