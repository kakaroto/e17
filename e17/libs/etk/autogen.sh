#!/bin/sh

rm -rf autom4te.cache
rm -f aclocal.m4

touch README

echo "Running aclocal..."; aclocal $ACLOCAL_FLAGS -I m4 \
&& echo "Running autoheader..."; autoheader \
&& echo "Running autoconf..."; autoconf \
&& echo "Running libtoolize..."; (libtoolize --copy --automake || glibtoolize --automake) \
&& echo "Running automake..."; automake --add-missing --copy --gnu \
&& echo "Generating gettext etk.pot template"; \
xgettext \
-n \
-C \
-d etk \
-p po \
--copyright-holder="Enlightenment development team" \
--foreign-user \
--msgid-bugs-address="enlightenment-devel@lists.sourceforge.net" \
-k -k_ -kd_ \
--from-code=UTF-8 \
-o etk.pot \
`find . -name "*.[ch]" -print`

if [ -z "$NOCONFIGURE" ]; then
	./configure "$@"
fi
