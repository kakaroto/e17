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
URL: http://www.enlightenment.org/pages/edb.html
Packager: Mark Bainter <mark-e@cymry.org>
Vendor: The Enlightenment Development Team
Source: ftp://ftp.enlightenment.org/enlightenment/e17/libs/%{name}-%{ver}.tar.gz
BuildRoot: /var/tmp/%{name}-root
Prefix: %{prefix}

%description
Edb is a database abstraction layer to Berkeley Databases. Edb contains the
source for DB 2.7.7, thus freezing the database format on disk, making sure it
will never become incompatible (as is a habit of the DB interface in libc). Edb
wraps this with a convenience and optimization API layer, making database
access easy, fast and consistent. It handles typing of information in the
database and much more.


%package devel
Summary: Enlightenment Database Access Library headers and development libraries.
Group: Development/Libraries
Requires: %{name} = %{ver}

%description devel
This package contains headers and libraries for compiling or developing
applications which depend on the edb libraries.  If you are only going
to run precompiled binaries you do not need this package.

%prep
%setup -q

%build
if [ -e ./configure ]
then
  ./configure --prefix=%{prefix}
else
  ./autogen.sh --prefix=%{prefix}
fi
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

%changelog
* Sat Aug 11 2001 Mark Bainter <mark-e@cymry.org>
- Made package relocatable.
- Corrected Web and FTP URLs.
- Took out the (wrong) email address for the development team.
- Expended the package descriptions
