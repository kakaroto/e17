# Note that this is NOT a relocatable package
%define ver      0.0.1
%define rel      1
%define prefix   /usr

Summary: epeg
Name: epeg
Version: %ver
Release: %rel
Copyright: BSD
Group: System Environment/Libraries
Source: ftp://ftp.enlightenment.org/pub/epeg/epeg-%{ver}.tar.gz
BuildRoot: /var/tmp/epeg-root
Packager: The Rasterman <raster@rasterman.com>
URL: http://www.enlightenment.org/
BuildRequires: libjpeg-devel
Requires: libjpeg

Docdir: %{prefix}/doc

%description

Epeg is a Canvas Server

%package devel
Summary: Epeg headers, static libraries, documentation and test programs
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
%attr(755,root,root) %{prefix}/lib/libepeg.so*
%attr(755,root,root) %{prefix}/lib/libepeg.la

%files devel
%attr(755,root,root) %{prefix}/lib/libepeg.a
%attr(755,root,root) %{prefix}/bin/epeg*
%{prefix}/include/Epeg*
%doc AUTHORS
%doc COPYING
%doc README
%doc epeg_docs.tar.gz

%changelog
* Wed Oct 29 2003 The Rasterman <raster@rasterman.com>
- Created spec file
