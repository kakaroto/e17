%define name	enlightenment
%define version 0.16.7
%define release 0.44

Summary:	The Enlightenment window manager.
Name:		%{name}
Version:	%{version}
Release:	%{release}
Copyright:	BSD
Group:		User Interface/Desktops
Source:		ftp://ftp.enlightenment.org/pub/enlightenment/enlightenment-%{version}.tar.gz
Prefix:		%{_prefix}
Docdir:		%{_docdir}
BuildRoot:	/tmp/e-%{version}-root
Packager:	Michael Jennings <mej@eterm.org>
URL:		http://www.enlightenment.org/

Requires: imlib2
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
CFLAGS="${RPM_OPT_FLAGS}"
ENLIGHTENMENT_RELEASE=%{release}
export CFLAGS ENLIGHTENMENT_RELEASE
if [ ! -f configure ]; then
  ./autogen.sh --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir} --mandir=%{_mandir} --enable-fsstd --enable-upgrade=no --enable-hints-gnome
else
  %{configure} --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir} --mandir=%{_mandir} --enable-fsstd --enable-upgrade=no --enable-hints-gnome
fi
make

%install
rm -rf $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS BUGS COPYING INSTALL README FAQ
%{_datadir}/enlightenment/*
%{_datadir}/locale/*/LC_MESSAGES/enlightenment.mo
%{_bindir}/*
%{_mandir}/man1/*
