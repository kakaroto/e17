# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
EFL_PKG_IUSE="doc test"

inherit efl

DESCRIPTION="Enlightenment's data types library (List, hash, etc) in C."
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Eina"
SRC_URI=""

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS=""

IUSE="mmx sse sse2 altivec +threads +safety-checks
	+mempool-chained mempool-fixed-bitmap +mempool-pass-through mempool-buddy
	mempool-ememoa-fixed mempool-ememoa-unknown
	default-mempool +static-modules debug voltron"

RDEPEND="
	mempool-ememoa-fixed? ( sys-libs/ememoa )
	mempool-ememoa-unknown? ( sys-libs/ememoa )"

DEPEND="
	${RDEPEND}
	test? (
		dev-libs/glib
		dev-util/lcov
	)"

src_configure() {
	local DEBUG_FLAGS="" SAFETY_CHECKS_FLAGS="" TEST_FLAGS="" EMEMOA_FLAGS=""

	# ???: should we use 'use_enable' for these as well?
	if use debug; then
		DEBUG_FLAGS="
		  --disable-amalgamation
		  --enable-stringshare-usage
		  --enable-assert
		"
	else
		DEBUG_FLAGS="
		  --enable-amalgamation
		  --disable-stringshare-usage
		  --disable-assert
		  --with-internal-maximum-log-level=2
		"
	fi

	if use safety-checks || use debug; then
		SAFETY_CHECKS_FLAGS="
		  --enable-magic-debug
		  --enable-safety-checks
		"
	else
		ewarn "Compiling without safety-checks is dangerous and unsupported."
		ewarn "Just use safety-checks if you really know what you are doing."
		SAFETY_CHECKS_FLAGS="
		  --disable-magic-debug
		  --disable-safety-checks
		"
	fi

	if use test; then
		if ! use safety-checks; then
			die "Cannot run tests without safety-checks, otherwise negative tests will crash."
			die "Compile with USE=safety-checks"
		fi

		# Evas benchmark is broken!
		TEST_FLAGS="
		  --enable-e17
		  --enable-tests
		  --enable-coverage
		  --enable-benchmark"
		#these flags don't even exist
#		  --disable-benchmark-evas
#		  --enable-benchmark-glib
	fi

	if use static-modules; then
		MODULE_ARGUMENT="static"
	else
		MODULE_ARGUMENT="yes"
	fi

	if use mempool-ememoa-fixed || use mempool-ememoa-unknown; then
		EMEMOA_FLAGS="--enable-ememoa"
	else
		EMEMOA_FLAGS="--disable-ememoa"
	fi

	export MY_ECONF="
	  ${MY_ECONF}
	  $(use_enable voltron)
	  $(use_enable mmx cpu-mmx)
	  $(use_enable sse cpu-sse)
	  $(use_enable sse2 cpu-sse2)
	  $(use_enable altivec cpu-altivec)
	  $(use_enable threads posix-threads)
	  $(use_enable mempool-chained mempool-chained-pool $MODULE_ARGUMENT)
	  $(use_enable mempool-ememoa-fixed mempool-ememoa-fixed $MODULE_ARGUMENT)
	  $(use_enable mempool-ememoa-unknown mempool-ememoa-unknown $MODULE_ARGUMENT)
	  $(use_enable mempool-fixed-bitmap mempool-fixed-bitmap $MODULE_ARGUMENT)
	  $(use_enable mempool-pass-through mempool-pass-through $MODULE_ARGUMENT)
	  $(use_enable mempool-buddy mempool-buddy $MODULE_ARGUMENT)
	  $(use_enable default-mempool)
	  ${DEBUG_FLAGS}
	  ${SAFETY_CHECKS_FLAGS}
	  ${TEST_FLAGS}
	  ${EMEMOA_FLAGS}
	"

	efl_src_configure
}
