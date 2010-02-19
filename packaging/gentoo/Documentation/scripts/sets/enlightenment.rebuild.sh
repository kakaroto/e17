#!/bin/sh

# Script done by Andrian Nord and imported from:
# http://git.niifaq.ru/enlightenment-niifaq/tree/Documentation/scripts/portage/update.etc.portage.file.sh

ROOT=$(portageq envvar ROOT)
REPO_ROOT=$(portageq get_repo_path "${ROOT}" efl)
installed=$(portageq match "${ROOT}" "");

for ebuild in $(find "${REPO_ROOT}" -name "*-9999.ebuild" | sed -r "s|^.*/([[:alnum:]_\.\-]+)/[^\/]+/([[:alnum:]_\.\-]+)\.ebuild|\1/\2|" ); do
	echo "${installed}" | grep --color=never "${ebuild}" | sed 's/^/=/';
done
