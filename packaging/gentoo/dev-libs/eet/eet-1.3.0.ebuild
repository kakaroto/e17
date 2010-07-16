# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
EFL_PKG_IUSE="doc test"

inherit efl

SRC_URI="http://download.enlightenment.org/releases/${P}.tar.bz2"

DESCRIPTION="E file chunk reading/writing library"
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Eet"
SRC_URI="http://download.enlightenment.org/releases/${P}.tar.gz"

LICENSE="BSD"
SLOT="0"
KEYWORDS="~alpha ~amd64 ~arm ~hppa ~ia64 ~mips ~ppc ~ppc64 ~sh ~sparc ~x86 ~x86-fbsd"

IUSE="+threads debug gnutls ssl"

RDEPEND="media-libs/jpeg
	dev-libs/eina
	sys-libs/zlib
	gnutls? ( net-libs/gnutls )
	!gnutls? ( ssl? ( dev-libs/openssl ) )"

DEPEND="${RDEPEND}"

src_configure() {
	local SSL_FLAGS="" DEBUG_FLAGS="" TEST_FLAGS=""

	# ???: should we use 'use_enable' for these as well?
	if use debug; then
		DEBUG_FLAGS="
		  --disable-amalgamation
		  --enable-assert
		"
	else
		DEBUG_FLAGS="
		  --enable-amalgamation
		  --disable-assert
		"
	fi

	if use test; then
		TEST_FLAGS="
		  --enable-tests
		  --enable-coverage
		"
	fi

	if use gnutls; then
		if use ssl; then
			ewarn "You have enabled both 'ssl' and 'gnutls', so we will use"
			ewarn "gnutls and not openssl for cipher and signature support"
		fi
		SSL_FLAGS="
		  --enable-cipher
		  --enable-signature
		  --disable-openssl
		  --enable-gnutls
		"
	elif use ssl; then
		SSL_FLAGS="
		  --enable-cipher
		  --enable-signature
		  --enable-openssl
		  --disable-gnutls
		"
	else
		SSL_FLAGS="
		  --disable-cipher
		  --disable-signature
		  --disable-openssl
		  --disable-gnutls
		"
	fi

	export MY_ECONF="
	  ${MY_ECONF}
	  $(use_enable threads posix-threads)
	  ${SSL_FLAGS}
	  ${DEBUG_FLAGS}
	  ${TEST_FLAGS}
	"

	efl_src_configure
}
