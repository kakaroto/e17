# Note that this is NOT a relocatable package
%define ver      0.0.1
%define rel      1
%define prefix   /usr

Summary: elation
Name: elation
Version: %ver
Release: %rel
Copyright: BSD
Group: System Environment/Libraries
Source: ftp://ftp.enlightenment.org/pub/elation/elation-%{ver}.tar.gz
BuildRoot: /var/tmp/elation-root
Packager: The Rasterman <raster@rasterman.com>
URL: http://www.enlightenment.org/

Docdir: %{prefix}/doc

%description

Elation is a Media Player centered around being pvr/dvr-like

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

%files
%defattr(-,root,root)
%attr(755,root,root) %{prefix}/bin/elation*
%attr(755,root,root) %{prefix}/lib/elation/*
%attr(755,root,root) %{prefix}/share/elation/data/theme.edc
%attr(755,root,root) %{prefix}/share/elation/data/theme.eet

%changelog
* Sat Jun 23 2001 The Rasterman <raster@rasterman.com>
- Created spec file                                            
