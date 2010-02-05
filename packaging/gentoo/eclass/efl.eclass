# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /var/cvsroot/gentoo-x86/eclass/enlightenment.eclass,v 1.80 2009/03/07 22:28:16 vapier Exp $
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

if [[ ${WANT_AUTOTOOLS} == "yes" ]] ; then
	: ${WANT_AUTOCONF:=${E_WANT_AUTOCONF:-latest}}
	: ${WANT_AUTOMAKE:=${E_WANT_AUTOMAKE:-latest}}

	inherit autotools
fi

HOMEPAGE="http://www.enlightenment.org/"
case ${EURI_STATE:-${E_STATE}} in
	release) SRC_URI="mirror://sourceforge/enlightenment/${P}.tar.gz";;
	snap)    SRC_URI="http://download.enlightenment.org/snapshots/${E_SNAP_DATE}/${P}.tar.bz2";;
	live)    SRC_URI="";;
esac

LICENSE="BSD"
SLOT="0"

case ${EKEY_STATE:-${E_STATE}} in
	release) KEYWORDS="alpha amd64 arm hppa ia64 mips ppc ppc64 sh sparc x86 ~x86-fbsd";;
	snap)    KEYWORDS="~alpha ~amd64 ~arm ~hppa ~ia64 ~mips ~ppc ~ppc64 ~sh ~sparc ~x86 ~x86-fbsd";;
	live)    KEYWORDS="";;
esac

DEPEND="${DEPEND} dev-util/pkgconfig"
MY_ECONF=""

if [[ -z "${E_NO_NLS}" ]]; then
	IUSE="${IUSE} nls"
	DEPEND="${DEPEND} nls? ( sys-devel/gettext )"

	# gettext (via `autopoint`) needs to run cvs #245073
	if [[ ${E_STATE} == "live" ]] && [[ ${WANT_AUTOTOOLS} == "yes" ]]; then
		DEPEND="${DEPEND} dev-util/cvs"
	fi
fi

if [[ -z "${E_NO_DOC}" ]]; then
	IUSE="${IUSE} doc"
	DEPEND="${DEPEND} doc? ( app-doc/doxygen )"
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
		eerror "If it fails to build, FIX THE CODE YOURSELF"
		eerror "before reporting any issues."
	fi
}

efl_die() {
	efl_warning_msg
	die "$@"$'\n'"!!! SEND BUG REPORTS TO enlightenment@gentoo.org NOT THE E TEAM"
}

efl_src_test() {
	if use test; then
		emake -j1 check || die "Make check failed. see above for details"
	fi
}

# the stupid gettextize script prevents non-interactive mode, so we hax it
gettext_modify() {
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

	if [[ -e configure.ac || -e configure.in ]] && \
		[[ "${WANT_AUTOTOOLS}" == "yes" ]]; then
		if grep -qE '^[[:space:]]*AM_GNU_GETTEXT_VERSION' configure.*; then
			local autopoint_log_file="${T}/autopoint.$$"

			ebegin "Running autopoint"

			autopoint -f &> "${autopoint_log_file}"

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

		# someone forgot these very useful file...
		touch README

		eautoreconf
		local x
		for x in config.{guess,sub} ; do
			[[ ! -L ${x} ]] && continue
			rm -f ${x}
			touch ${x}
		done
	fi

	epunt_cxx
	elibtoolize
}

efl_src_configure() {
	if [[ -x ${ECONF_SOURCE:-.}/configure ]]; then
		[[ -z "${E_NO_NLS}" ]] && MY_ECONF="${MY_ECONF} $(use_enable nls)"
		[[ -z "${E_NO_DOC}" ]] && MY_ECONF="${MY_ECONF} $(use_enable doc)"

		econf --disable-static "${MY_ECONF}" || efl_die "configure failed"
	fi
}

efl_src_compile() {
	emake || efl_die "emake failed"

	if use doc; then
		if [[ -x ./gendoc ]]; then
			./gendoc || efl_die "gendoc failed"
		else
			emake doc
		fi
	fi
}

efl_src_install() {
	emake install DESTDIR="${D}" || efl_die

	find "${D}" -name .svn -type d -exec rm -rf '{}' \; 2>/dev/null
	find "${D}" -name '*.la' -delete

	for d in AUTHORS ChangeLog NEWS README TODO ${EDOCS}; do
		[[ -f ${d} ]] && dodoc ${d}
	done

	if use doc && [[ -d doc ]]; then
		if [[ -d doc/html ]]; then
			dohtml -r doc/html/*
		else
			dohtml -r doc/*
		fi
	fi
}

EXPORT_FUNCTIONS src_unpack src_prepare src_configure src_compile src_install src_test
