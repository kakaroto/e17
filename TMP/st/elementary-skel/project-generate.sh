#!/bin/bash

for a in "$@"; do
    if test "x$a" = "x-h" -o "x$a" = "x--help"; then
        LICENSES=`ls -1 COPYING.* | cut -d. -f2 | tr '\n' ' '`
        TEMPLATES=`ls -1 src/bin/main-*.c | cut -d- -f2- | cut -d. -f1 | tr '\n' ' '`

        cat <<EOF_USAGE
Usage:

   $0 <project-dir> <project-name> <description> <author> <email> <license> <main_template>

Where:
   <project-dir> where to generate project, must not exist.

   <project-name> desired project name just [a-zA-Z_-].

   <description> project description, may be multiline.

   <author> author's name.

   <email> author's email address.

   <license> is one of: $LICENSES

   <main_template> is one of: $TEMPLATES

EOF_USAGE
    fi
done

DIR=${1:?Missing directory}
PROJECT=${2:?Missing project name}
PROJECT_DESCRIPTION=${3:?Missing project description}
AUTHOR_NAME=${4:?Missing author name}
AUTHOR_EMAIL=${5:?Missing author email}
LICENSE=${6:?Missing license}
MAIN_TEMPLATE=${7:?Missing main template name}

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

if test ! -f "src/bin/main-${MAIN_TEMPLATE}.c"; then
    TEMPLATES=`ls -1 src/bin/main-*.c | cut -d- -f2- | cut -d. -f1 | tr '\n' ' '`
    die_simple "Unknown main template $MAIN_TEMPLATE, use one of src/bin/main-*.c ($TEMPLATES)"
fi

YEAR=`date +"%Y"`

PROJECT=`echo "$PROJECT" | sed 's/[^a-zA-Z0-9_-]/_/g'`
PROJECT_=`echo "$PROJECT" | sed 's/[^a-zA-Z0-9_]/_/g'`

if test -d "$DIR"; then
    die_simple " $DIR already exists."
fi

mkdir -p "$DIR" || die "mkdir $DIR failed"

PROJECT_DESCRIPTION_ESCAPED=`echo "$PROJECT_DESCRIPTION" | sed -e 's:/:\\\/:g' `

BLACKLIST_RE="\(~$\|COPYING[.]\|project-generate[.]sh\|src/bin/main-\)"
REPLACE_SED="s/@PROJECT@/$PROJECT/g;s/@PROJECT_@/$PROJECT_/g;s/@PROJECT_NAME@/$PROJECT_NAME/g;s/@PROJECT_DESCRIPTION@/$PROJECT_DESCRIPTION_ESCAPED/g;s/@LICENSE@/$LICENSE/g;s/@YEAR@/$YEAR/g;s/@AUTHOR_NAME@/$AUTHOR_NAME/g;s/@AUTHOR_EMAIL@/$AUTHOR_EMAIL/g"

cp_file_dst() {
    src="$1"
    dst="$2"
    echo "F: '$src' -> '$DIR/$dst'"
    sed "$src" -e "$REPLACE_SED" > "$DIR/$dst" || die "could not create '$DIR/$dst'"
}

cp_file() {
    src=${1:?missing file}
    if echo "$src" | grep -e "$BLACKLIST_RE" 2>/dev/null >/dev/null; then
        echo "I: '$src' (blacklisted)"
        return
    fi
    dst=`echo "$src" | sed -e "$REPLACE_SED"`
    cp_file_dst "$src" "$dst"
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
cp_file_dst "src/bin/main-$MAIN_TEMPLATE.c" "src/bin/main.c"

chmod +x "$DIR/autogen.sh" || die "could not chmod autogen.sh"

TEMPLATE_DESCRIPTION_STR=`cat src/bin/main-$MAIN_TEMPLATE.txt | sed 's/\(.\{,72\}\)\([ ]\|$\)/\1<br>/g;s/\(\(<br>\)\+\)/<br>/g;s/<br>/\\n/g'`

cat <<EOF

Created 'elementary' based project with the following parameters:

   Project..: $PROJECT
   Directory: $DIR
   Author...: $AUTHOR_NAME <$AUTHOR_EMAIL>
   Copyright: $YEAR $LICENSE
   Template.: $MAIN_TEMPLATE

-- Template Description: ------------------------------------------------
$TEMPLATE_DESCRIPTION_STR
-------------------------------------------------------------------------


-- Project Description: -------------------------------------------------
$PROJECT_DESCRIPTION
-------------------------------------------------------------------------

Now 'cd $DIR && ./autogen.sh && make && make install', have fun.

EOF
