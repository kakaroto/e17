MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = emphasis.edc \
				 haricot_eap.png \
				 build.sh
EDC_FILE = emphasis.edc

all: Makefile $(EDC_FILE)
	./build.sh

clean-local:
	rm -f *.eap
	rm -rf autom4te.cache
