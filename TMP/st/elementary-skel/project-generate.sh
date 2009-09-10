#!/bin/bash

DIR=${1:?Missing directory}
PROJECT=${2:?Missing project name}
PROJECT_DESCRIPTION=${3:?Missing project description}
AUTHOR_NAME=${4:?Missing author name}
AUTHOR_EMAIL=${5:?Missing author email}
LICENSE=${6:?Missing license}

die_simple() {
    echo "ERROR: $*" >&2
    exit 1
}

die() {
    echo "ERROR: $*" >&2
    echo -n "Clean $DIR? [Y/n]"
    read clean
    if test "x$clean" = "xn" -o "x$clean" = "xN"; then
        echo "Directory maintained: $DIR"
    else
        echo "Removing directory: $DIR"
        rm -fr "$DIR"
    fi
    exit 1
}

if test ! -f "COPYING.$LICENSE"; then
    LICENSES=`ls -1 COPYING.* | cut -d. -f2 | tr '\n' ' '`
    die_simple "Unknown license $LICENSE, use one of COPYING.* ($LICENSES)"
fi

YEAR=`date +"%Y"`

PROJECT=`echo "$PROJECT" | sed 's/[^a-zA-Z0-9_-]/_/g'`
PROJECT_=`echo "$PROJECT" | sed 's/[^a-zA-Z0-9_]/_/g'`

if test -d "$DIR"; then
    die_simple " $DIR already exists."
fi

mkdir -p "$DIR" || die "mkdir $DIR failed"

PROJECT_DESCRIPTION_ESCAPED=`echo "$PROJECT_DESCRIPTION" | sed -e 's:/:\\\/:g' `

BLACKLIST_RE="\(~$\|COPYING[.]\|project-generate[.]sh\)"
REPLACE_SED="s/@PROJECT@/$PROJECT/g;s/@PROJECT_@/$PROJECT_/g;s/@PROJECT_NAME@/$PROJECT_NAME/g;s/@PROJECT_DESCRIPTION@/$PROJECT_DESCRIPTION_ESCAPED/g;s/@LICENSE@/$LICENSE/g;s/@YEAR@/$YEAR/g;s/@AUTHOR_NAME@/$AUTHOR_NAME/g;s/@AUTHOR_EMAIL@/$AUTHOR_EMAIL/g"

cp_file() {
    src=${1:?missing file}
    if echo "$src" | grep -e "$BLACKLIST_RE" 2>/dev/null >/dev/null; then
        echo "I: '$src' (blacklisted)"
        return
    fi
    dst=`echo "$src" | sed -e "$REPLACE_SED"`
    echo "F: '$src' -> '$DIR/$dst'"
    sed "$src" -e "$REPLACE_SED" > "$DIR/$dst" || die "could not create '$DIR/$dst'"
}

cp_dir() {
    if test -z "$1"; then
        BASE="."
        EXP="*"
    else
        BASE="$1"
        EXP="$BASE/*"
        mkdir -p "$DIR/$BASE" || die "could not mkdir '$DIR/$BASE'"
        echo "D: '$BASE' -> '$DIR/$BASE'"
    fi
    for f in $EXP; do
        if test -f "$f"; then
            cp_file "$f"
        elif test -d "$f"; then
            cp_dir "$f"
        fi
    done
}

cp_dir

cp "COPYING.$LICENSE" "$DIR/COPYING" || die "could not copy license file."
chmod +x "$DIR/autogen.sh" || die "could not chmod autogen.sh"


cat <<EOF

Created 'elementary' based project with the following parameters:

   Project..: $PROJECT
   Author...: $AUTHOR_NAME <$AUTHOR_EMAIL>
   Copyright: $YEAR $LICENSE

   Directory: $DIR

Description: -----------------------------------------------------------

$PROJECT_DESCRIPTION

------------------------------------------------------------------------

Now 'cd $DIR && ./autogen.sh && make && make install', have fun.

EOF
