AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-Wall -Wextra -Wno-missing-field-initializers \
-I$(top_srcdir) \
-I$(top_srcdir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPACKAGE_TMP_DIR=\"/tmp\" \
@ELEMENTARY_CFLAGS@

moddir = $(libdir)/$(PACKAGE)/$(MODULE_ARCH)
mod_LTLIBRARIES = libelev8_elm.la

libelev8_elm_la_SOURCES = \
	Main.cc \
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
	CElmDaySelector.cc \
	CEvasImage.cc \
	CEvasObject.cc

libelev8_elm_la_LIBADD = @ELEMENTARY_LIBS@ @V8_LIBS@
libelev8_elm_la_LDFLAGS = -module -avoid-version -no-undefined
libelev8_elm_la_LIBTOOLFLAGS = --tag=disable-static
