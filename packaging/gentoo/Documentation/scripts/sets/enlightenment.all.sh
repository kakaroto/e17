#!/bin/sh

# Script done by Andrian Nord and imported from:
# http://git.niifaq.ru/enlightenment-niifaq/tree/Documentation/scripts/sets/enlightenment.all.sh

ROOT="$(portageq envvar ROOT)"
REPO_ROOT="$(portageq get_repo_path ${ROOT} efl)"

BROKEN="${REPO_ROOT}/sets/efl-broken"

for ebuild in $(find "${REPO_ROOT}" -name "*-9999.ebuild" | sed -r "s|^.*/([[:alnum:]_\.\-]+)/[^\/]+/([[:alnum:]_\.\-]+)\.ebuild|=\1/\2|"); do
	grep -q "${ebuild}" "${BROKEN}" || echo ${ebuild};
done;
