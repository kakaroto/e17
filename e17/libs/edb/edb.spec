Summary: Enlightenment Database Access Library
Name: edb
Version: 1.0.3
Release: 1
Copyright: BSD
Group: System Environment/Libraries
URL: http://www.enlightenment.org/pages/edb.html
Packager: Mark Bainter <mark-e@cymry.org>
Vendor: The Enlightenment Development Team (http://www.enlightenment.org/)
Source: ftp://ftp.enlightenment.org/enlightenment/e17/libs/%{name}-%{version}.tar.gz
BuildRoot: /var/tmp/%{name}-root

%description
Edb is a database abstraction layer to Berkeley Databases. Edb contains the
source for DB 2.7.7, thus freezing the database format on disk, making sure it
will never become incompatible (as is a habit of the DB interface in libc). Edb
wraps this with a convenience and optimization API layer, making database
access easy, fast and consistent. It handles typing of information in the
database and much more.

%prep
%setup -q

%build
if [ -e ./configure ]
then
  %{configure} --prefix=%{_prefix} --bindir=%{_bindir} --libdir=%{_libdir} --includedir=%{_includedir}
else
  ./autogen.sh --prefix=%{_prefix} --bindir=%{_bindir} --libdir=%{_libdir} --includedir=%{_includedir}
fi
make

%install
make DESTDIR=$RPM_BUILD_ROOT install

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc AUTHORS COPYING README src/LICENSE
%{_libdir}/libedb.so.*
%{_libdir}/libedb.so
%{_libdir}/libedb.*a
%{_includedir}/Edb.h
%{_bindir}/edb*

%changelog
* Sat Aug 11 2001 Mark Bainter <mark-e@cymry.org>
- Made package relocatable.
- Corrected Web and FTP URLs.
- Took out the (wrong) email address for the development team.
- Expended the package descriptions
