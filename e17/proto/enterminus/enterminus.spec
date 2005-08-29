# Note that this is NOT a relocatable package
%define ver      0.1
%define rel      1
%define prefix   /usr

Summary: enterminus
Name: enterminus
Version: %ver
Release: %rel
Copyright: BSD
Group: System Environment/Libraries
Source: ftp://ftp.enlightenment.org/pub/enterminus/enterminus-%{ver}.tar.gz
BuildRoot: /var/tmp/enterminus-root
Packager: The Rasterman <raster@rasterman.com>
URL: http://www.enlightenment.org/
BuildRequires: libjpeg-devel
BuildRequires: zlib-devel
Requires: libjpeg
Requires: zlib

Docdir: %{prefix}/doc

%description

enterminus is a terminal emulator based on the EFL

%package devel
Summary: enterminus headers, static libraries, documentation and test programs
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
%attr(755,root,root) %{prefix}/lib/libenterminus.so*
%attr(755,root,root) %{prefix}/lib/libenterminus.la

%files devel
%attr(755,root,root) %{prefix}/lib/libenterminus.a
%attr(755,root,root) %{prefix}/bin/enterminus*
%{prefix}/include/enterminus*
%doc AUTHORS
%doc COPYING
%doc README
%doc enterminus_docs.tar.gz

%changelog
* Sat Jun 23 2001 The Rasterman <raster@rasterman.com>
- Created spec file
