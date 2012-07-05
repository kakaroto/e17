cformat() {
	ecrustify -c svn/trunk/FORMATTING/ecrustify.cfg --replace $@
	sed -i 's/FOREACH (/FOREACH(/g;s/FREE (/FREE(/g' $@
}
hformat() {
	ecrustify -c svn/trunk/FORMATTING/ecrustify-headers.cfg --replace $@
	sed -i 's/FOREACH (/FOREACH(/g;s/FREE (/FREE(/g' $@
}
