# Note that this is NOT a relocatable package
%define ver      0.16.6
%define rel      1
%define prefix   /usr

Summary: The Enlightenment window manager. Crippled for GNOME ;-)
Name: enlightenment_for_GNOME
Version: %ver
Release: %rel
Copyright: GPL
Group: User Interface/Desktops
Source: ftp://ftp.enlightenment.org/pub/enlightenment/enlightenment-%{ver}.tar.gz
BuildRoot: /tmp/e-%{ver}-root
Packager: Tom Gilbert <gilbertt@linuxbrit.co.uk>
URL: http://www.enlightenment.org
Requires: imlib >= 1.9.8
Requires: fnlib >= 0.5
Requires: freetype >= 1.1
Requires: esound >= 0.2.13

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

This version has had a number of GNOME-friendly customisations.
Several options are disabled by default because they are
duplicated by GNOME. They can of course be re-enabled at any
time.

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
  CFLAGS="$RPM_OPT_FLAGS" ./autogen.sh --prefix=%prefix --enable-fsstd --enable-upgrade=no
else
  CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%prefix --enable-fsstd --enable-upgrade=no
fi
make

%install
rm -rf $RPM_BUILD_ROOT

make prefix=$RPM_BUILD_ROOT%{prefix} install

( cd $RPM_BUILD_ROOT/%{prefix}/share/enlightenment/config
  mv control.cfg control.cfg.real_E
  mv control.rh.cfg control.cfg
  
  mv keybindings.cfg keybindings.cfg_real_E
  mv keybindings.rh.cfg keybindings.cfg

  mv menus.cfg menus.cfg_real_E
  mv menus.rh.cfg menus.cfg
)

%clean
rm -rf $RPM_BUILD_ROOT

%post

%postun

%files
%defattr(-, root, root)

%{prefix}/share/enlightenment/*
%{prefix}/bin/*
%{prefix}/man/man1/*

%doc AUTHORS
%doc COPYING
%doc INSTALL
%doc README
%doc FAQ
%doc TODO
