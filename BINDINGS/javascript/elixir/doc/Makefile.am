# -*- Makefile -*-

DOCREF			= doxygen_ref.tar.gz

MAINTAINERCLEANFILES	= $(DOCREF) Makefile.in
EXTRA_DIST		= $(DOCREF)
docdir			= $(datadir)/doc/${PACKAGE}
#doc_DATA		= $(DOCREF)

$(DOCREF): $(srcdir)/doc.doxy.in
	if test -d $(srcdir)/.svn -a -x `which svn`; then \
	  REV=`LANG=C svn info $(srcdir) | sed -ne 's/^Revision: *\([0-9]\+\)/r\1/p'`; \
	  sed -e "s/%%Revision%%/$$REV/" < doc.doxy > doc.doxy.rev; \
	  $(DOXYGEN) doc.doxy.rev; \
	elif test "`svk info $(top_srcdir) 2> /dev/null`" != ""; then \
	  REV=`svk info $(top_srcdir) 2> /dev/null | sed -ne 's/^Mirrored From: .*, Rev. *\([0-9]\+\)/r\1/p'`; \
	  sed -e "s/%%Revision%%/$$REV/" < doc.doxy > doc.doxy.rev; \
	  $(DOXYGEN) doc.doxy.rev; \
	fi
	if ! test -d doxygen_html; then \
	  sed -e "s/%%Revision%%//" < doc.doxy > doc.doxy.rev; \
	  $(DOXYGEN) doc.doxy.rev; \
	fi
	@rm -f doc.doxy.rev
	tar czf $@ doxygen_html

clean-local::
	rm -rf doxygen_html
