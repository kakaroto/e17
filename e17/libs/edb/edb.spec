%define	name	edb
%define	ver	1.0.2
%define	rel	1
%define prefix  /usr

Summary: Enlightenment Database Access Library
Name: %{name}
Version: %{ver}
Release: %{rel}
Copyright: BSD
Group: System Environment/Libraries
URL: http://www.enlightenment.org/efm.html
Packager: The Rasterman <raster@rasterman.com>
Vendor: The Enlightenment Development Team <e-develop@enlightenment.org>
Source: ftp://ftp.enlightenment.org/enlightenment/%{name}-%{ver}.tar.gz
BuildRoot: /var/tmp/%{name}-root

%description
Edb is a simple, clean high-level db access/storage library.

%package devel
Summary: Enlightenment Database Access Library headers and development libraries.
Group: Development/Libraries
Requires: %{name} = %{ver}

%description devel
Edb development headers and libraries.

%prep
%setup -q

%build
./configure --prefix=%{prefix}
make

%install
make prefix=$RPM_BUILD_ROOT%{prefix} install

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{prefix}/lib/libedb.so.*
%{prefix}/bin/edb_*

%doc AUTHORS
%doc COPYING
%doc README
%doc src/LICENSE

%files devel
%defattr(-,root,root)
%{prefix}/lib/libedb.so
%{prefix}/lib/libedb.*a
%{prefix}/include/Edb.h
%{prefix}/bin/edb-config


