Summary: Enlightenment Database Access Library
Name: edb
Version: 1.0.5
Release: 2.%(date '+%Y%m%d')
Copyright: BSD
Group: System Environment/Libraries
URL: http://www.enlightenment.org/pages/edb.html
Packager: Michael Jennings <mej@eterm.org>
Vendor: The Enlightenment Development Team (http://www.enlightenment.org/)
Source: ftp://ftp.enlightenment.org/enlightenment/e17/libs/%{name}-%{version}.tar.gz
#BuildSuggests: gtk-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
Edb is a database abstraction layer to Berkeley Databases. Edb
contains the source for DB 2.7.7, thus freezing the database format on
disk, making sure it will never become incompatible (as is a habit of
the DB interface in libc). Edb wraps this with a convenience and
optimization API layer, making database access easy, fast and
consistent. It handles typing of information in the database and much
more.

%package devel
Summary: Edb headers, static libraries, documentation and test programs
Group: System Environment/Libraries
Requires: %{name} = %{version}

%description devel
Headers, static libraries, test programs and documentation for Edb

%package ed
Summary: Edb command-line editor
Group: System Environment/Libraries
Requires: %{name} = %{version}

%description ed
A command-line db editor for Edb

%package gtk_ed
Summary: Edb command-line editor
Group: System Environment/Libraries
Requires: %{name} = %{version}

%description gtk_ed
A GTK+ gui db editor for Edb

%package vt_ed
Summary: Edb command-line editor
Group: System Environment/Libraries
Requires: %{name} = %{version}

%description vt_ed
A curses db editor for Edb

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q

%build
if [ -e ./configure ]
then
  %{configure} --prefix=%{_prefix} --bindir=%{_bindir} --libdir=%{_libdir} --includedir=%{_includedir}
else
  ./autogen.sh --prefix=%{_prefix} --bindir=%{_bindir} --libdir=%{_libdir} --includedir=%{_includedir}
fi
%{__make} %{?_smp_mflags} %{?mflags}

%install
%{__make} %{?mflags_install} DESTDIR=$RPM_BUILD_ROOT install

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING README src/LICENSE
%{_libdir}/libedb.so.*
%{_libdir}/libedb.la

%files devel
%defattr(-, root, root)
%{_libdir}/libedb.so
%{_libdir}/libedb.a
%{_bindir}/edb-config
%{_libdir}/pkgconfig/edb.pc
%{_includedir}/Edb*

%files ed
%defattr(-, root, root)
%{_bindir}/edb_ed

%files gtk_ed
%defattr(-, root, root)
%{_bindir}/edb_gtk_ed

%files vt_ed
%defattr(-, root, root)
%{_bindir}/edb_vt_ed

%changelog
