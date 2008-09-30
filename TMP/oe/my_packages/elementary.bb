DESCRIPTION = "An EFL based Alarm app"
LICENSE = "MIT BSD"
DEPENDS = "evas ecore edje eet edbus"
PV = "0.0.0+svnr${SRCREV}"
PR = "r0.20"

inherit efl

SRC_URI = "svn://svn.enlightenment.org/svn/e/trunk/TMP/st;module=elementary;proto=http"
S = "${WORKDIR}/elementary"

FILES_${PN}  = "${bindir}/* ${libdir}/lib*.so.* ${libdir}/${PN}"
FILES_${PN} += "${datadir}/${PN}"
FILES_${PN} += "${datadir}/${PN}/themes/*"
FILES_${PN} += "${datadir}/${PN}/images/*"

