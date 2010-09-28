# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /var/cvsroot/gentoo-x86/dev-libs/eet/eet-9999.ebuild,v 1.4 2005/03/25 17:51:29 vapier Exp $

EAPI=2

inherit enlightenment

DESCRIPTION="E file chunk reading/writing library"
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Eet"

RDEPEND=">=dev-libs/eina-9999
	media-libs/jpeg
	sys-libs/zlib
	gnutls? ( net-libs/gnutls )
	!gnutls? ( openssl? ( dev-libs/openssl ) )"
DEPEND="${RDEPEND}
	test? ( dev-libs/check
		dev-util/lcov )"

IUSE="debug examples gnutls openssl static-libs +threads"

src_configure() {
	local SSL_FLAGS=""

	if use gnutls; then
		if use openssl; then
			ewarn "You have enabled both 'openssl' and 'gnutls', so we will use"
			ewarn "gnutls and not openssl for cipher and signature support"
		fi
		SSL_FLAGS="
			--enable-cipher
			--enable-signature
			--disable-openssl
			--enable-gnutls"
	elif use openssl; then
		SSL_FLAGS="
			--enable-cipher
			--enable-signature
			--enable-openssl
			--disable-gnutls"
	else
		SSL_FLAGS="
			--disable-cipher
			--disable-signature
			--disable-openssl
			--disable-gnutls"
	fi

	export MY_ECONF="
		$(use_enable !debug amalgamation)
		$(use_enable debug assert)
		$(use_enable doc)
		$(use_enable test tests)
		$(use_enable test coverage)
		$(use_enable threads posix-threads)
		${SSL_FLAGS}
		${MY_ECONF}"

	enlightenment_src_configure
}

src_install() {
	enlightenment_src_install
	rm -r src/examples/Makefile* || die
	docinto examples
	dodoc src/examples/* || die
}
