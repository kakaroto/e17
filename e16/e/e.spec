# Note that this is NOT a relocatable package
%define ver      0.16.1
%define rel      1
%define prefix   /usr

Summary: The Enlightenment window manager.
Name: enlightenment
Version: %ver
Release: %rel
Copyright: GPL
Group: User Interface/Desktops
Source: ftp://www.rasterman.com/pub/enlightenment/enlightenment-%{ver}.tar.gz
BuildRoot: /tmp/e-%{ver}-root
Packager: The Rasterman <raster@redhat.com>
URL: http://www.rasterman.com/
Requires: imlib >= 1.9.5
Requires: fnlib >= 0.4
Requires: freetype >= 1.1
Requires: esound >= 0.2.12

Docdir: %{prefix}/doc

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
# Optimize that damned code all the way
if [ ! -z "`echo -n ${RPM_OPT_FLAGS} | grep pentium`" ]; then
  if [ ! -z "`which egcs`" ]; then
    CC="egcs" 
  else
    if [ ! -z "`which pgcc`" ]; then
      CC="pgcc"
    fi
  fi
  CFLAGS="${RPM_OPT_FLAGS}"
else
  CFLAGS="${RPM_OPT_FLAGS}"
fi
if [ ! -f configure ]; then
  CFLAGS="$RPM_OPT_FLAGS" ./autogen.sh --prefix=%prefix --enable-fsstd
else
  CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%prefix --enable-fsstd
fi
make

%install
rm -rf $RPM_BUILD_ROOT

make prefix=$RPM_BUILD_ROOT%{prefix} install

cd $RPM_BUILD_ROOT%{prefix}/
chown -R 0.0 *

%clean
rm -rf $RPM_BUILD_ROOT

%post

%postun

%files
%defattr(-, root, root)

%{prefix}/share/enlightenment/*
%{prefix}/bin/*

%doc AUTHORS
%doc COPYING
%doc INSTALL
%doc README
%doc FAQ
%doc TODO
