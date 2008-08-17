Summary:	Enlightenment theme: winter
Name:		e16-theme-winter
Version:	0.16.8
Release:	0.%(date '+%y%m%d')
License:	BSD
Group:		User Interface/Desktops
Source0:	http://prdownloads.sourceforge.net/enlightenment/%{name}-%{version}.tar.gz
Prefix:		%{_prefix}
BuildArch:	noarch
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root
URL:		http://www.enlightenment.org/

Requires: e16 >= 0.16.8

%description
The winter theme for Enlightenment.
This is part of the Enlightenment distribution.

%prep
%setup

%build
CFLAGS="${RPM_OPT_FLAGS}"
if [ ! -f configure ]; then
  ./autogen.sh --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir} --mandir=%{_mandir} --enable-fsstd
else
  %{configure} --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir} --mandir=%{_mandir} --enable-fsstd
fi
make

%install
rm -rf $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING INSTALL README
%{_datadir}/e16/*

%changelog

