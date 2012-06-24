THEMEDIR ?= /usr/share/exquisite/data/themes

slackware.edj: slackware.edc
	edje_cc -v -id ./images -fd ./fonts $< $@

install: slackware.edj
	mkdir -p $(DESTDIR)$(THEMEDIR)
	install -m644 $< $(DESTDIR)$(THEMEDIR)/

uninstall:
	rm -f $(DESTDIR)$(THEMEDIR)/slackware.edj

clean: 
	rm -f *~ slackware.edj
