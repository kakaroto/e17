# Note that this is NOT a relocatable package
%define ver      0.0.1
%define rel      1
%define prefix   /usr

Summary: evoak
Name: evoak
Version: %ver
Release: %rel
Copyright: BSD
Group: System Environment/Libraries
Source: ftp://ftp.enlightenment.org/pub/evoak/evoak-%{ver}.tar.gz
BuildRoot: /var/tmp/evoak-root
Packager: The Rasterman <raster@rasterman.com>
URL: http://www.enlightenment.org/
BuildRequires: libjpeg-devel
BuildRequires: zlib-devel
Requires: libjpeg
Requires: zlib

Docdir: %{prefix}/doc

%description

Evoak is a Canvas Server

%package devel
Summary: Evoak headers, static libraries, documentation and test programs
Group: System Environment/Libraries
Requires: %{name} = %{version}

%description devel
Headers, static libraries, test programs and documentation for Eet

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
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(-,root,root)
%attr(755,root,root) %{prefix}/lib/libevoak.so*
%attr(755,root,root) %{prefix}/lib/libevoak.la

%files devel
%attr(755,root,root) %{prefix}/lib/libevoak.a
%attr(755,root,root) %{prefix}/bin/evoak*
%{prefix}/include/Evoak*
%doc AUTHORS
%doc COPYING
%doc README
%doc evoak_docs.tar.gz

%changelog
* Sat Jun 23 2001 The Rasterman <raster@rasterman.com>
- Created spec file
