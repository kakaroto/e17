Summary: The Enlightenment window manager.
Name: enlightenment
Version: 0.16.6
Release: 0.13
Copyright: BSD
Group: User Interface/Desktops
Source: ftp://ftp.enlightenment.org/pub/enlightenment/enlightenment-%{version}.tar.gz
Prefix: %{_prefix}
Docdir: %{_docdir}
BuildRoot: /tmp/e-%{version}-root
Packager: Michael Jennings <mej@eterm.org>
URL: http://www.enlightenment.org/
Requires: imlib >= 1.9.8
Requires: fnlib >= 0.5
Requires: esound >= 0.2.13

%description
Enlightenment is a window manager for the X Window System that
is designed to be powerful, extensible, configurable and
pretty darned good looking! It is one of the more graphically
intense window managers.

Enlightenment goes beyond managing windows by providing a useful
and appealing graphical shell from which to work. It is open
in design and instead of dictating a policy, allows the user to 
define their own policy, down to every last detail.

This package will install the Enlightenment window manager.

%changelog

%prep
%setup

%build
CFLAGS="-O0 -g3"
#CFLAGS="${RPM_OPT_FLAGS}"
export CFLAGS
if [ ! -f configure ]; then
  ./autogen.sh --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir} --mandir=%{_mandir} --enable-fsstd --enable-upgrade=no --enable-hints-kde
else
  %{configure} --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir} --mandir=%{_mandir} --enable-fsstd --enable-upgrade=no --enable-hints-kde
fi
make

%install
rm -rf $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING INSTALL README FAQ
%{_datadir}/enlightenment/*
%{_bindir}/*
%{_mandir}/man1/*
