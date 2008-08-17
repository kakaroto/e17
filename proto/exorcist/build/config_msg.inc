# print types
boldface="`tput bold 2>/dev/null`"
normal="`tput sgr0 2>/dev/null`"

print_bold() {
    echo -n $ECHO_N "$boldface"
    echo -n "$@"
    echo -n $ECHO_N "$normal"
}

error_std_headers_msg() {
    echo
    print_bold "ERROR:"
    echo
    echo "  Can not find standard headers"
    echo ". Make sure that your CPPFLAGS"
    echo "  environment variable contains the include lines"
    echo "  for the location of this file."
    echo
    exit 1
}

error_header_msg() {
    echo
    print_bold "ERROR:"
    echo
    echo -n "  Can not find "
    print_bold $1
    echo ". Make sure that your CPPFLAGS"
    echo "  environment variable contains the include lines"
    echo "  for the location of this file."
    echo
    exit 1
}

error_library_msg() {
    echo
    print_bold "ERROR:"
    echo
    echo -n "  Can not find the library "
    print_bold $1
    echo ". Make sure that your"
    echo "  CPPFLAGS and LDFLAGS environment variable contains the"
    echo "  include lines for the location of this file."
    echo
    exit 1
}

error_toolkit_msg() {
    echo
    print_bold "ERROR:"
    echo
    echo "  Can not find the ewl-config or etk-config."
    echo "  Make sure that these scripts are in the \$PATH"
    echo "  environment variable. Run configure with the"
    echo "  option --with-ewl-config or --with-etk-config"
    echo "  to specify the configure script."
    echo
    exit 1
}