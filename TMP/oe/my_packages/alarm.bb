DESCRIPTION = "An EFL based Alarm app"
LICENSE = "GPL"
DEPENDS = "evas ecore edje eet"
PV = "0.0.0+svnr${SRCREV}"
PR = "r0.10"

inherit e

SRC_URI = "svn://svn.enlightenment.org/svn/e/trunk/TMP/st;module=alarm;proto=http"
S = "${WORKDIR}/alarm"

PACKAGES="${PN}-dbg ${PN}"

FILES_${PN}  = "${bindir}/* ${libdir}/lib*.so.* ${libdir}/${PN}"
FILES_${PN} += "${datadir}/${PN}"
FILES_${PN} += "${datadir}/icons/*"
FILES_${PN} += "${datadir}/applications/*"
FILES_${PN} += "${datadir}/sounds/*"
FILES_${PN} += "${datadir}/images/*"
FILES_${PN} += "/etc/X11/Xsession.d/80x-enlightenment-wakerd"
