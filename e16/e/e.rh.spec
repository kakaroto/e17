Summary: The Enlightenment window manager.
Name: enlightenment
Version: 0.16.6
Release: 0.18
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

This version has had a number of GNOME-friendly customisations.
Several options are disabled by default because they are duplicated by
GNOME. They can of course be re-enabled at any time.

%changelog

%prep
%setup

%build
CFLAGS="${RPM_OPT_FLAGS}"
export CFLAGS
if [ ! -f configure ]; then
  ./autogen.sh --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir} --mandir=%{_mandir} --enable-fsstd --enable-upgrade=no
else
  %{configure} --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir} --mandir=%{_mandir} --enable-fsstd --enable-upgrade=no
fi
make

%install
rm -rf $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT install

(
  cd $RPM_BUILD_ROOT/%{_prefix}/share/enlightenment/config
  mv control.cfg control.cfg.real_E
  mv control.rh.cfg control.cfg
  
  mv keybindings.cfg keybindings.cfg_real_E
  mv keybindings.rh.cfg keybindings.cfg

  mv menus.cfg menus.cfg_real_E
  mv menus.rh.cfg menus.cfg
)

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING INSTALL README FAQ
%{_datadir}/enlightenment/*
%{_datadir}/locale/*/LC_MESSAGES/enlightenment.mo
%{_bindir}/*
%{_mandir}/man1/*
