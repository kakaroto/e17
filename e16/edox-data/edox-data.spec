Summary:	Documentation for the Enlightenment window manager.
Name:		enlightenment-docs
Version:	0.16.7
Release:	0.03
License:	BSD
Group:		User Interface/Desktops
Source0:	http://prdownloads.sourceforge.net/enlightenment/%{name}-%{version}.tar.gz
Prefix:		%{_prefix}
BuildArch:	noarch
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root
URL:		http://www.enlightenment.org/

Requires: enlightenment >= 0.16.7
Obsoletes: edox-data

%description
These are the DOX data files.
DOX is a very very very simple document viewer for help docs for Enlightenment.
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
%{_datadir}/enlightenment/*

%changelog

* Mon May 24 2004 Stuart Children <stuart@terminus.co.uk> - 0.16.7-0.03
- Renamed from edox-data to enlightenment-docs.

* Mon May 10 2004 Stuart Children <stuart@terminus.co.uk> - 0.16.7-0.02
- Tidy ups.
- Fix download url.
- Use License rather than Copyright.
- Remove Packager to avoid people creating mis-attributed packages.

