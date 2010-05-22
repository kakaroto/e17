MAINTAINERCLEANFILES = Makefile.in

.PHONY: doc

if EFL_BUILD_DOC

doc: all
	rm -rf html/ latex/ man/
	$(efl_doxygen)
	cp images/* html/

else

doc:
	@echo "Documentation not built. Run ./configure --help"

endif

clean-local:
	@rm -rf html/ latex/ man/

EXTRA_DIST = Doxyfile $(wildcard images/*.*) e.css head.html foot.html
