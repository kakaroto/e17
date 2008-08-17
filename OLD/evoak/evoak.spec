%define _missing_doc_files_terminate_build 0

Summary: Evas-based Canvas Server
Name: evoak
Version: 0.0.1
Release: 1.%(date '+%Y%m%d')
License: BSD
Group: System Environment/Libraries
URL: http://www.enlightenment.org/
Source: ftp://ftp.enlightenment.org/pub/evoak/%{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:Michael Jennings <mej@eterm.org>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
#BuildSuggests: xorg-x11-devel
BuildRequires: libjpeg-devel zlib-devel XFree86-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
Evoak is a canvas server. This is similar to an X server that serves
out a display and graphics operations. Evoak serves out a single
canvas to be shared by multiple applications (clients) allowing each
client to manipulate its set of objects.

%package devel
Summary: Evoak headers, static libraries, documentation and test programs
Group: System Environment/Libraries
Requires: %{name} = %{version}

%description devel
Headers, static libraries, test programs and documentation for Eet

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
%doc AUTHORS COPYING* README
%{_libdir}/libevoak.so*
%{_libdir}/libevoak.la

%files devel
%defattr(-, root, root)
%doc doc/html
%{_libdir}/libevoak.a
%{_bindir}/evoak*
%{_includedir}/Evoak*
%{_datadir}/evoak

%changelog
