%define ver      0.0.1
%define rel      1
%define prefix   /usr

Summary: examine
Name: examine
Version: %ver
Release: %rel
Copyright: BSD
Group: System Environment/Libraries
Source: ftp://ftp.enlightenment.org/pub/examine/examine-%{ver}.tar.gz
BuildRoot: /var/tmp/examine-root
Packager: Andrew Elcock <andy@elcock.org>
URL: http://www.enlightenment.org/
Requires: ecore
Requires: edb

Docdir: %{prefix}/doc

%description

Examine is a Configuration Interface

%package devel
Summary: Ecore_Config client programs
Group: System Environment/Libraries
Requires: %{name} = %{version}

%description devel
programs and documentation for Ecore_Config clients

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
%{prefix}/bin/exsh
%{prefix}/bin/examine
%doc AUTHORS
%doc COPYING
%doc README

%changelog
* Thu Dec 02 2004 Azundris <hacks@azundris.com>
- There is are no docs at this stage, so don't package a useless docs.tgz
* Sat Jun 23 2001 The Rasterman <raster@rasterman.com>
- Created spec file
