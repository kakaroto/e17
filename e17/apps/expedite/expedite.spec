# Note that this is NOT a relocatable package
%define ver      0.6.0
%define rel      1
%define prefix   /usr

Summary: expedite
Name: expedite
Version: %ver
Release: %rel
Copyright: BSD
Group: System Environment/Desktops
Source: ftp://ftp.enlightenment.org/pub/enlightenment/expedite-%{ver}.tar.gz
BuildRoot: /var/tmp/expedite-root
Packager: The Rasterman <raster@rasterman.com>
URL: http://www.enlightenment.org/
BuildRequires: evas-devel
Requires: evas

%description

Expedite Evas benchmark/test suite

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q

%build
./configure --prefix=%prefix

if [ "$SMP" != "" ]; then
  (make "MAKE=make -k -j $SMP"; exit 0)
  make
else
  make
fi
###########################################################################

%install
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%post

%postun

%files
%defattr(-,root,root)
%attr(755,root,root) %{prefix}/bin/*
%attr(755,root,root) %{prefix}/share/expedite
%doc AUTHORS
%doc COPYING
%doc COPYING-PLAIN
%doc README

%changelog
* Sat Jun 23 2001 The Rasterman <raster@rasterman.com>
- Created spec file
