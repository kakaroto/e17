#!/bin/sh

rm -rf autom4te.cache
rm -f aclocal.m4

echo "Running aclocal..."; aclocal $ACLOCAL_FLAGS -I m4\
&& echo "Running autoheader..."; autoheader \
&& echo "Running autoconf..."; autoconf \
&& echo "Running libtoolize..."; ( libtoolize --automake || glibtoolize --automake ) \
&& echo "Running automake..."; automake --add-missing --copy --gnu && \
echo "Generating gettext evolume.pot template" && \
xgettext \
--output evolume.pot \
--output-dir=po \
--language=C \
--add-location \
--keyword=D_ \
--indent \
--sort-by-file \
--copyright-holder="Pavel Boldin" \
--foreign-user \
`find . -name "*.[ch]" -print` || exit 1

if [ -z "$NOCONFIGURE" ]; then
	./configure "$@"
fi
