Summary: Enlightened Text Object Library
Name: etox
Version: 0.9.0
Release: 1
Copyright: BSD
Group: User Interface/X
URL: http://www.enlightenment.org/pages/etox.html
Source: ftp://ftp.enlightenment.org/enlightenment/%{name}-%{version}.tar.gz
Packager: Michael Jennings <mej@eterm.org>
Vendor: The Enlightenment Development Team <e-develop@enlightenment.org>
Requires: evas >= 1.0.0
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
Etox is a type setting and text layout library based on Evas. Etox
helps you when it comes to displaying, moving, resizing, layering,
clipping, aligning and coloring fonts in different styles.

Among other things, Etox provides a text layout engine that can
dynamically arrange text flow around other graphical obstacles.

%package devel
Summary: Etox headers and development libraries.
Group: Development/Libraries
Requires: %{name} = %{version}

%description devel
Etox development headers and libraries.

%prep
%setup -q

%build
%{configure} --prefix=%{_prefix}
%{__make} %{?_smp_mflags} %{?mflags}

%install
%{__make} %{?mflags_install} DESTDIR=$RPM_BUILD_ROOT install
test -x `which doxygen` && sh gendoc || :

%post
/sbin/ldconfig || :

%postun
/sbin/ldconfig || :

%clean
test "x$RPM_BUILD_ROOT" != "x/" && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING* README*
%{_libdir}/libetox.so.*
%{_libdir}/libetox.la
%{_bindir}/etox_test
%{_bindir}/etox_selections
%{_datadir}/etox/*

%files devel
%defattr(-, root, root)
%doc doc/html
%{_libdir}/libetox.so
%{_libdir}/libetox.a
%{_includedir}/Etox.h
%{_bindir}/etox-config
%{_datadir}/aclocal/etox.m4
