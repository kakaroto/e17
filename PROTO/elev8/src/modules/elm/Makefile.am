AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPACKAGE_TMP_DIR=\"/tmp\" \
@ELEMENTARY_CFLAGS@

moddir = $(libdir)/$(PACKAGE)/$(MODULE_ARCH)
mod_LTLIBRARIES = libelm.la

libelm_la_SOURCES = \
	main.cc \
	CElmActionSlider.cc \
	CElmBackground.cc \
	CElmBox.cc \
	CElmBubble.cc \
	CElmButton.cc \
	CElmCalendar.cc \
	CElmCheck.cc \
	CElmClock.cc \
	CElmColorSelector.cc \
	CElmDaySelector.cc \
	CElmEntry.cc \
	CElmFlip.cc \
	CElmHover.cc \
	CElmIcon.cc \
	CElmImage.cc \
	CElmInwin.cc \
	CElmLabel.cc \
	CElmObject.cc \
	CElmPane.cc \
	CElmPhoto.cc \
	CElmProgressBar.cc \
	CElmRadio.cc \
	CElmScroller.cc \
	CElmSegment.cc \
	CElmSlider.cc \
	CElmSpinner.cc \
	CElmWindow.cc

#	Main.cc \
#	CElmBasicWindow.cc \
#	CElmConform.cc \
#	CElmCtxPopup.cc \
#	CElmDateTime.cc \
#	CElmFileSelectorButton.cc \
#	CElmFileSelectorEntry.cc \
#	CElmFileSelector.cc \
#	CElmGenList.cc \
#	CElmGrid.cc \
#	CElmLayout.cc \
#	CElmNaviframe.cc \
#	CElmNotify.cc \
#	CElmPhotocam.cc \
#	CElmTable.cc \
#	CEvasImage.cc \
#	CEvasObject.cc

libelm_la_LIBADD = @ELEMENTARY_LIBS@ @V8_LIBS@
libelm_la_LDFLAGS = -module -avoid-version -no-undefined
libelm_la_LIBTOOLFLAGS = --tag=disable-static
