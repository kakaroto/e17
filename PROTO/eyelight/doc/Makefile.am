
MAINTAINERCLEANFILES = Makefile.in

.PHONY: doc

if EFL_BUILD_DOC

doc: all
	rm -rf html/ latex/ man/
	$(efl_doxygen) $(top_srcdir)/doc
	cp $(top_srcdir)/doc/img/* $(top_builddir)/doc/html/

else

doc:
	@echo "Documentation not built. Run ./configure --help"

endif

clean-local:
	@rm -rf html/ latex/ man/

EXTRA_DIST = Doxyfile $(top_srcdir)/doc/img/*.* e.css head.html foot.html
