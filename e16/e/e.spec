Summary:	The Enlightenment window manager.
Name:		enlightenment
Version:	0.16.7.1
Release:	0.03
License:	BSD
Group:		User Interface/Desktops
Source0:	http://prdownloads.sourceforge.net/enlightenment/%{name}-%{version}.tar.gz
Prefix:		%{_prefix}
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root
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
%doc AUTHORS COPYING ChangeLog INSTALL README FAQ
%{_datadir}/enlightenment/*
%{_datadir}/locale/*/LC_MESSAGES/enlightenment.mo
%{_bindir}/*
%{_mandir}/man1/*
%{_sysconfdir}/X11/dm/Sessions/*

%changelog

* Mon May 10 2004 Stuart Children <stuart@terminus.co.uk> - 0.16.7-0.59
- Tidy ups.
- Fix download url.
- Use License rather than Copyright.
- Remove Packager to avoid people creating mis-attributed packages.

