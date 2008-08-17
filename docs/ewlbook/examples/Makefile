
SUBDIRS = create_window \
	hello_world

all:
	list='$(SUBDIRS)'; \
	for i in $$list; do \
	  (cd $$i && $(MAKE));\
	done

clean:
	list='$(SUBDIRS)'; \
	for i in $$list; do \
	  (cd $$i && $(MAKE) clean); \
	done

