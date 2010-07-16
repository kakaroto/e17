# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $
#
# Author: vapier@gentoo.org
# Modified: barbieri@profusion.mobi
# Modified: NightNord@gmail.com

inherit eutils libtool flag-o-matic

# E_STATE's:
#	release      [default]
#		KEYWORDS arch
#		SRC_URI  $P.tar.gz
#		S        $WORKDIR/$P
#
#	snap         $PV has .200##### datestamp or .### counter
#		KEYWORDS ~arch
#		SRC_URI  $P.tar.bz2
#		S        $WORKDIR/$P
#
#	live         $PV has a 9999 marker
#		KEYWORDS ""
#		SRC_URI  svn/etc... up
#		S        $WORKDIR/$E_S_APPEND
#
# Overrides:
#	KEYWORDS    EKEY_STATE
#	SRC_URI     EURI_STATE
#	S           EURI_STATE
#
# E_NO_NLS: if defined, the package do not support NLS (gettext)
# E_NO_DOC: if defined, the package do not support documentation (doxygen)
# E_NO_VISIBILITY: if defined, the package do not support -fvisibility=hidden
# E_NO_DISABLE_STATIC: if defined, do not append --disable-static
#
# Python support:
# E_PYTHON: if defined, the package is Python/distutils
# E_CYTHON: if defined, the package is Cython bindings (implies E_PYTHON)
# E_NO_EXAMPLES: if defined, the Python package does not provide examples

E_LIVE_SERVER_DEFAULT_SVN="http://svn.enlightenment.org/svn/e/trunk"

E_STATE="release"

if [[ ${PV/9999} != ${PV} ]] ; then
	E_STATE="live"
	PROPERTIES="live"

	: ${WANT_AUTOTOOLS:=yes}

	[[ -n ${E_LIVE_OFFLINE} ]] && ESCM_OFFLINE="yes"

	E_LIVE_SERVER=${E_LIVE_SERVER:-${E_LIVE_SERVER_DEFAULT_SVN}}
	ESVN_URI_APPEND=${ESVN_URI_APPEND:-${PN}}
	ESVN_PROJECT="enlightenment/${ESVN_SUB_PROJECT}"
	ESVN_REPO_URI=${E_LIVE_SERVER}/${ESVN_SUB_PROJECT}/${ESVN_URI_APPEND}

	E_S_APPEND=${ESVN_URI_APPEND}
	inherit subversion
elif [[ -n ${E_SNAP_DATE} ]] ; then
	E_STATE="snap"
else
	E_STATE="release"
fi

if [[ ! -z "${E_CYTHON}" ]]; then
	E_PYTHON="1"
fi

if [[ ! -z "${E_PYTHON}" ]]; then
	WANT_AUTOTOOLS="no"
	WANT_AUTOCONF="no"
	WANT_AUTOMAKE="no"

	E_NO_VISIBILITY="1"

	NEED_PYTHON="2.4"

	inherit python distutils
fi

if [[ ${WANT_AUTOTOOLS} == "yes" ]] ; then
	: ${WANT_AUTOCONF:=${E_WANT_AUTOCONF:-latest}}
	: ${WANT_AUTOMAKE:=${E_WANT_AUTOMAKE:-latest}}

	inherit autotools
fi

DEPEND="${DEPEND} dev-util/pkgconfig"

if [[ -z "${E_NO_NLS}" ]]; then
	IUSE="${IUSE} nls"
	DEPEND="${DEPEND} nls? ( sys-devel/gettext )"

	# gettext (via `autopoint`) needs to run cvs #245073
	if [[ ${E_STATE} == "live" ]] && [[ ${WANT_AUTOTOOLS} == "yes" ]]; then
		DEPEND="${DEPEND} dev-vcs/cvs"
	fi
fi

if [[ -z "${E_NO_DOC}" ]]; then
	IUSE="${IUSE} doc"
	DEPEND="${DEPEND} doc? ( app-doc/doxygen )"
fi

if [[ ! -z "${E_CYTHON}" ]]; then
	DEPEND="${DEPEND} >=dev-python/cython-0.12.1"
fi

if [[ ! -z "${E_PYTHON}" ]]; then
	DEPEND="${DEPEND} >=dev-python/setuptools-0.6_rc9"

	if [[ -z "${E_NO_EXAMPLES}" ]]; then
		IUSE="${IUSE} examples"
	fi
fi

if [[ -z "${E_NO_VISIBILITY}" ]] && [[ $(gcc-major-version) -ge 4 ]]; then
	append-flags -fvisibility=hidden
fi

case ${EURI_STATE:-${E_STATE}} in
	release) S=${WORKDIR}/${P};;
	snap)    S=${WORKDIR}/${P};;
	live)    S=${WORKDIR}/${E_S_APPEND};;
esac

efl_warning_msg() {
	if [[ -n ${E_LIVE_SERVER} ]] ; then
		einfo "Using user server for live sources: ${E_LIVE_SERVER}"
	fi

	if [[ ${E_STATE} == "snap" ]] ; then
		ewarn "Please do not contact the E team about bugs in Gentoo."
		ewarn "Only contact enlightenment@gentoo.org via e-mail or bugzilla."
		ewarn "Remember, this stuff is DEV only code so dont cry when"
		ewarn "I break you :)."
	elif [[ ${E_STATE} == "live" ]] ; then
		eerror "This is a LIVE SOURCES ebuild."
		eerror "That means there are NO promises it will work."
		eerror "If it fails to build, contact the enlightenment mailing list"
		eerror "or go to #e on FreeNode irc for support."
	fi
}

efl_die() {
	efl_warning_msg
	die "$@"$'\n'"!!! SEND BUG REPORTS TO enlightenment@gentoo.org NOT THE E TEAM"
}

efl_src_test() {
	if [[ -z "${E_PYTHON}" ]]; then
		if use test; then
			emake -j1 check || die "Make check failed. see above for details"
		fi
	fi
}

# the stupid gettextize script prevents non-interactive mode, so we hax it
gettext_modify() {
	[[ -z "${E_NO_NLS}" ]] || return 0
	use nls || return 0
	cp $(type -P gettextize) "${T}"/ || die "could not copy gettextize"
	sed -i \
		-e 's:read dummy < /dev/tty::' \
		"${T}"/gettextize
}

efl_src_unpack() {
	if [[ ${E_STATE} == "live" ]] ; then
		subversion_src_unpack
	else
		unpack ${A}
	fi

	cd "${S}"
}

efl_src_prepare() {
	gettext_modify

	[[ -s gendoc ]] && chmod a+rx gendoc

	if [[ -z "${E_PYTHON}" ]]; then
		if [[ -e configure.ac || -e configure.in ]] && \
			[[ "${WANT_AUTOTOOLS}" == "yes" ]]; then

			export SVN_REPO_PATH="$ESVN_WC_PATH"

			if [[ -z "${E_NO_NLS}" ]] && \
				grep -qE '^[[:space:]]*AM_GNU_GETTEXT_VERSION' configure.{ac,in}; then
				local autopoint_log_file="${T}/autopoint.$$"

				ebegin "Running autopoint"

				eautopoint -f &> "${autopoint_log_file}"

				if ! eend $?; then
					ewarn "Autopoint failed"
					ewarn "Log in ${autopoint_log_file}"
					ewarn "(it makes sense only when compile fails afterwards)"
				fi

				if grep -qi 'cvs program not found' "${autopoint_log_file}"; then
					ewarn "This error seems to be due missing CVS"
					ewarn "(it's usage hardcoded into autopoint code)"
					ewarn "Please 'emerge cvs' if compilation will fail"
					ebeep 3
				fi
			fi

			# autotools expect README, when README.in is around, but README
			# is created later in configure step
			[[ -f README.in ]] && touch README

			eautoreconf
			local x
		fi

		epunt_cxx
		elibtoolize
	fi
}

efl_src_configure() {
	if [[ -z "${E_PYTHON}" ]]; then
		export SVN_REPO_PATH="$ESVN_WC_PATH"
		if [[ -x ${ECONF_SOURCE:-.}/configure ]]; then
			[[ -z "${E_NO_NLS}" ]] && MY_ECONF="${MY_ECONF} $(use_enable nls)"
			[[ -z "${E_NO_DOC}" ]] && MY_ECONF="${MY_ECONF} $(use_enable doc)"
			[[ -z "${E_NO_DISABLE_STATIC}" ]] && MY_ECONF="${MY_ECONF} --disable-static"

			econf ${MY_ECONF} || efl_die "configure failed"
		fi
	fi
}

efl_src_compile() {
	if [[ -z "${E_PYTHON}" ]]; then
		emake || efl_die "emake failed"

		if [[ -z "${E_NO_DOC}" ]] && use doc; then
			if [[ -x ./gendoc ]]; then
				./gendoc || efl_die "gendoc failed"
			else
				emake doc
			fi
		fi
	else
		distutils_src_compile
		if [[ -z "${E_NO_DOC}" ]] && use doc; then
			if [[ -x ./gendoc ]]; then
				./gendoc || efl_die "gendoc failed"
			fi
		fi
	fi
}

efl_src_install() {
	if [[ -z "${E_PYTHON}" ]]; then

		emake install DESTDIR="${D}" || efl_die

		find "${D}" -name .svn -type d -exec rm -rf '{}' \; 2>/dev/null
		find "${D}" -name '*.la' -delete

		for d in AUTHORS ChangeLog NEWS README TODO ${EDOCS}; do
			[[ -f ${d} ]] && dodoc ${d}
		done
	else
		distutils_src_install

		if [[ -z "${E_NO_EXAMPLES}" ]] && use examples; then
			insinto /usr/share/doc/${PF}
			doins -r examples

			find "${D}/usr/share/doc/${PF}" -name .svn -type d -exec rm -rf '{}' \; 2>/dev/null
		fi
	fi

	if [[ -z "${E_NO_DOC}" ]] && use doc && [[ -d doc ]]; then
		if [[ -d doc/html ]]; then
			dohtml -r doc/html/*
		else
			dohtml -r doc/*
		fi
		find "${D}/usr/share/doc/${PF}" -name .svn -type d -exec rm -rf '{}' \; 2>/dev/null
	fi
}

EXPORT_FUNCTIONS src_unpack src_prepare src_configure src_compile src_install src_test
