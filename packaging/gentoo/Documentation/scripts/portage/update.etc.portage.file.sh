#!/bin/sh

# Script done by Andrian Nord and imported from:
# http://git.niifaq.ru/enlightenment-niifaq/tree/Documentation/scripts/portage/update.etc.portage.file.sh

if [ -e /lib/rc/sh/functions.sh ]; then
	source /lib/rc/sh/functions.sh
elif [ -e /sbin/functions.sh ]; then
	source /sbin/functions.sh
else
	echo "Script supposed to be launched into Gentoo Linux system with openrc/baselayout"
	exit 1;
fi

usage() {
	local message="$@";

	if ! [[ -z "${message}" ]]; then
		eerror "${HILITE}Error:${NORMAL} ${message}"
		echo
	fi

	cat <<USAGE | expand -t 8
${GOOD}Usage:${NORMAL} $0 ${HILITE}-o|--output${NORMAL}=FILE [options]
${GOOD}Options:${NORMAL}
	${HILITE}-h|--help${NORMAL}			- this message
	${HILITE}-o|--output${NORMAL}=FILE		- put result into ${HILITE}FILE${NORMAL} (required)

	${HILITE}-t|--type${NORMAL}={unmask|keywords}	- produce output suitable for /etc/portage.${HILITE}TYPE${NORMAL}
	${HILITE}-u|--unmask${NORMAL}			- same as --type=unmask
	${HILITE}-k|--keywords${NORMAL}			- same as --type=keywords

	${HILITE}-v|--version${NORMAL}=VERSION		- collect packages with this ${HILITE}VERSION${NORMAL} (default: 9999)
	${HILITE}-l|--live${NORMAL}			- same as --version=9999

	${HILITE}--overlay${NORMAL}=NAME		- produce output for overlay with repo_name ${HILITE}NAME${NORMAL}
USAGE

	exit 1;
}

run_command() {
	local command="$@"

	[[ -z "${command}" ]] && read -r command;

	einfo "${HILITE}Running:${NORMAL} ${command}"

	eval ${command}
}

main() {
	local output= type='keywords' version=9999 overlay='efl'
	local ROOT="${ROOT:-/}"

	eval set -- x $(getopt -n "${HILITE}$0${NORMAL}" -o ho:ukt:v: -l help -l output: -l type: -l unmask,keywords -l version: -l live -l overlay: -- "$@"); shift;

	while [ $# -gt 0 ]; do
		case $1 in
			-h|--help)	usage;;
			-o|--output)	output=$2; shift;;

			-t|--type)	type=$2; shift;;
			-u|--unmask)	type='unmask';;
			-k|--keywords)	type='keywords';;

			-v|--version)	version=$2; shift;;
			-l|--live)	version=9999;;

			--overlay)	overlay=$2; shift;;

			--) break;;
		esac;

		shift;
	done

	einfo "${HILITE}Output:${NORMAL} ${output}"

	[[ -z "${output}" ]] && usage 'No output specified';

	[[ -w "$(dirname ${output})" ]] || usage "Output destination not writable";

	local overlay_dir=$(portageq get_repo_path "${ROOT}" "${overlay}")

	einfo "${HILITE}Overlay dir:${NORMAL} ${overlay_dir}"

	[[ "${overlay_dir}" == "None" ]]  && usage "Overlay not found";

	local format=
	einfo "${HILITE}Type:${NORMAL} ${type}"

	case ${type} in
		unmask) format="=\1/\2";;
		keywords) format="=\1/\2 **";;
		*) usage "File type '${type}' not supported"
	esac

	einfo $(echo "${HILITE}Output format:${NORMAL} '${format}'" | sed 's/\\1/<category>/; s/\\2/<package>-<version>/')
	einfo "${HILITE}Version:${NORMAL} ${version}"

	run_command <<CMD
find "${overlay_dir}" -name "*-${version}.ebuild" | sed -r "s|${overlay_dir}/([[:alnum:]_\-]+)/[^/]+/([[:alnum:]_\-]+).ebuild$|${format}|" | sort -u > "${output}"
CMD
}

main "$@"
