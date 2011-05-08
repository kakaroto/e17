CC?=gcc
EDJE_CC?=edje_cc
CFLAGS?=
LIBS?=
LDFLAGS?=
CFLAGS+=`pkg-config --cflags elementary edje evas`
LIBS+=`pkg-config --libs elementary edje evas`

all: tb_test tb.edj

tb_test: test.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) test.c -o tb_test

tb.edj: tb.edc
	$(EDJE_CC) tb.edc

clean:
	rm tb.edj tb_test
