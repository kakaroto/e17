%define	name	edb
%define	ver	0.0.1
%define	rel	1
%define prefix  /usr

Summary: Enlightened Database Access Library
Name: %{name}
Version: %{ver}
Release: %{rel}
Copyright: BSD
Group: User Interface/X
URL: http://www.enlightenment.org/efm.html
Packager: Term <kempler@utdallas.edu>
Vendor: The Enlightenment Development Team <e-develop@enlightenment.org>
Source: ftp://ftp.enlightenment.org/enlightenment/%{name}-%{ver}.tar.gz
BuildRoot: /var/tmp/%{name}-root

%description
Edb is a simple, clean high-level wrapper to ndbm.

%package devel
Summary: Enlightened Database Access Library headers and development libraries.
Group: Development/Libraries
Requires: %{name} = %{ver}

%description devel
Edb development headers and libraries.

%prep
%setup -q

%build
./autogen.sh --prefix=%{prefix}
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


