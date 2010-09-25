# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit enlightenment

DESCRIPTION="Enlightenment's (Ecore/Evas) video integration."
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Emotion"

# vlc/gstreamer support is buggy, do not even expose them here
#IUSE="gstreamer xine vlc static-modules"
IUSE="gstreamer xine static-modules static-libs"

# TODO: remove edje dependency as soon as emotion is fixed to not build its test
RDEPEND="
	>=dev-libs/eina-9999
	>=dev-libs/ecore-9999
	>=media-libs/evas-9999
	>=media-libs/edje-9999
	xine? ( >=media-libs/xine-lib-1.1.1 )
	gstreamer? (
		=media-libs/gstreamer-0.10*
		=media-libs/gst-plugins-good-0.10*
		=media-plugins/gst-plugins-ffmpeg-0.10*
	)"
#	vlc? media-video/vlc"
DEPEND="${RDEPEND}"

src_configure() {
#uncomment when functional
#	if ! use xine && ! use gstreamer && ! use vlc; then
	if ! use xine && ! use gstreamer; then
		die "Emotion needs at least one media system to be useful!"
		die "Compile media-libs/emotion with USE=xine or gstreamer."
	fi

	if use static-modules; then
		MODULE_ARGUMENT="static"
	else
		MODULE_ARGUMENT="yes"
	fi

	MY_ECONF="
	  --disable-vlc
	  $(use_enable xine xine $MODULE_ARGUMENT)
	  $(use_enable gstreamer gstreamer $MODULE_ARGUMENT)
	"
	# work around GStreamer's desire to check registry, which by default
	# results in sandbox access violation.
	export GST_REGISTRY="${T}"/registry.xml
	export GST_PLUGIN_SYSTEM_PATH="${T}"

	enlightenment_src_configure
}
