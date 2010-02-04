# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /var/cvsroot/gentoo-x86/eclass/enlightenment.eclass,v 1.80 2009/03/07 22:28:16 vapier Exp $
#
# Author: vapier@gentoo.org
# Modified: barbieri@profusion.mobi

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
	E_LIVE_SERVER=${E_LIVE_SERVER:-${E_LIVE_SERVER_DEFAULT_SVN}}
	E_STATE="live"
	WANT_AUTOTOOLS="yes"

	ESVN_URI_APPEND=${ESVN_URI_APPEND:-${PN}}
	ESVN_PROJECT="enlightenment/${ESVN_SUB_PROJECT}"
	ESVN_REPO_URI=${ESVN_SERVER:-${E_LIVE_SERVER_DEFAULT_SVN}}/${ESVN_SUB_PROJECT}/${ESVN_URI_APPEND}
	E_S_APPEND=${ESVN_URI_APPEND}
	E_LIVE_SOURCE="svn"
	inherit subversion
elif [[ -n ${E_SNAP_DATE} ]] ; then
	E_STATE="snap"
else
	E_STATE="release"
fi
if [[ ${WANT_AUTOTOOLS} == "yes" ]] ; then
	WANT_AUTOCONF=${E_WANT_AUTOCONF:-latest}
	WANT_AUTOMAKE=${E_WANT_AUTOMAKE:-latest}
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
MY_ECONF="--disable-static"

if [[ -z "${E_NO_NLS}" ]]; then
	IUSE="${IUSE} nls"
	DEPEND="${DEPEND} nls? ( sys-devel/gettext )"
	MY_ECONF="${MY_ECONF} $(use_enable nls)"

	# gettext (via `autopoint`) needs to run cvs #245073
	[[ ${E_STATE} == "live" ]] && DEPEND="${DEPEND} dev-util/cvs"
fi

if [[ -z "${E_NO_DOC}" ]]; then
	IUSE="${IUSE} doc"
	DEPEND="${DEPEND} doc? ( app-doc/doxygen )"
	MY_ECONF="${MY_ECONF} $(use_enable doc)"
fi

if [[ -z "${E_NO_VISIBILITY}" ]]; then
	if [[ $(gcc-major-version) -ge 4 ]]; then
		append-flags -fvisibility=hidden
	fi
fi

if [[ "${E_STATE}" != "release" ]]; then
	IUSE="${IUSE} debug"
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
		ewarn "Only contact barbieri@profusion.mobi via e-mail or bugzilla."
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
	die "$@"$'\n'"!!! SEND BUG REPORTS TO barbieri@profusion.mobi NOT THE E TEAM"
}

efl_pkg_setup() {
	: efl_warning_msg
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
		case ${E_LIVE_SOURCE} in
			svn) subversion_src_unpack;;
			*)   die "eek!";;
		esac
	else
		unpack ${A}
	fi
	gettext_modify
	[[ -s gendoc ]] && chmod a+rx gendoc
}

efl_src_compile() {
	# gstreamer sucks, work around it doing stupid stuff
	export GST_REGISTRY="${S}/registry.xml"

	if use debug; then
		strip-flags
		append-flags -g
		if ! hasq nostrip $FEATURES && ! hasq splitdebug $FEATURES; then
			ewarn "Compiling with USE=debug but portage will strip binaries!"
			ewarn "Please use portage FEATURES=nostrip or splitdebug"
			ewarn "See http://www.gentoo.org/proj/en/qa/backtraces.xml"
		fi
	fi

	if [[ ! -e configure ]] ; then
		env \
			PATH="${T}:${PATH}" \
			NOCONFIGURE=yes \
			USER=blah \
			./autogen.sh \
			|| efl_die "autogen failed"
		# symlinked files will cause sandbox violation
		local x
		for x in config.{guess,sub} ; do
			[[ ! -L ${x} ]] && continue
			rm -f ${x}
			touch ${x}
		done
	elif [[ ${WANT_AUTOTOOLS} == "yes" ]] ; then
		eautoreconf
	fi
	epunt_cxx
	elibtoolize
	econf ${MY_ECONF} || efl_die "econf failed"
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
	find "${D}" '(' -name CVS -o -name .svn -o -name .git ')' -type d -exec rm -rf '{}' \; 2>/dev/null
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

efl_pkg_postinst() {
	: efl_warning_msg
}

EXPORT_FUNCTIONS pkg_setup src_unpack src_compile src_install pkg_postinst src_test
