AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-Wall -Wextra \
-I$(top_srcdir) \
-I$(top_srcdir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPACKAGE_TMP_DIR=\"/tmp\" \
@ELEMENTARY_CFLAGS@

moddir = $(libdir)/$(PACKAGE)/$(MODULE_ARCH)
mod_LTLIBRARIES = libelm.la

libelm_la_SOURCES = elev8_elm.cc \
	CElmActionSlider.cc \
	CElmBackground.cc \
	CElmBasicWindow.cc \
	CElmBox.cc \
	CElmBubble.cc \
	CElmButton.cc \
	CElmCalendar.cc \
	CElmCheck.cc \
	CElmClock.cc \
	CElmColorSelector.cc \
	CElmEntry.cc \
	CElmFileSelectorButton.cc \
	CElmFileSelectorEntry.cc \
	CElmFlip.cc \
	CElmGenList.cc \
	CElmGrid.cc \
	CElmHover.cc \
	CElmIcon.cc \
	CElmImage.cc \
	CElmInwin.cc \
	CElmLabel.cc \
	CElmLayout.cc \
	CElmMenu.cc \
	CElmNaviframe.cc \
	CElmNotify.cc \
	CElmPane.cc \
	CElmPhotocam.cc \
	CElmPhoto.cc \
	CElmProgressBar.cc \
	CElmRadio.cc \
	CElmScroller.cc \
	CElmSegment.cc \
	CElmSlider.cc \
	CElmSpinner.cc \
	CElmTable.cc \
	CElmToggle.cc \
	CEvasImage.cc \
	CEvasObject.cc

libelm_la_LIBADD = @ELEMENTARY_LIBS@ @V8_LIBS@
libelm_la_LDFLAGS = -module -avoid-version -no-undefined
libelm_la_LIBTOOLFLAGS = --tag=disable-static
