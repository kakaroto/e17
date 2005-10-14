Summary: E Virtual Filesystem
Name: evfs
Version: 0.0.1
Release: 0.%(date '+%Y%m%d')
License: BSD
Group: System Environment/Libraries
URL: http://www.enlightenment.org/
Source: ftp://ftp.enlightenment.org/pub/evfs/%{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:Michael Jennings <mej@eterm.org>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
BuildRequires: ecore-devel, edje-devel, esmart-devel, gamin-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
E Virtual Filesystem

%package devel
Summary: EVFS Headers
License: BSD
Group: System Environment/Libraries
Requires: %{name}

%description devel
EVFS header files.

%prep
%setup -q

%build
%{configure} --prefix=%{_prefix}
%{__make} %{?_smp_mflags} %{?mflags}

%install
%{__make} %{?mflags_install} DESTDIR=$RPM_BUILD_ROOT install

%post
/sbin/ldconfig || :

%postun
/sbin/ldconfig || :

%clean
test "x$RPM_BUILD_ROOT" != "x/" && rm -rf $RPM_BUILD_ROOT

%files                                           
%defattr(-, root, root)                          
%doc AUTHORS COPYING* README
%{_bindir}/%{name}*
%{_libdir}/lib%{name}.so*
%{_libdir}/%{name}

%files devel
%{_includedir}/%{name}.h
%{_includedir}/%{name}
%{_libdir}/lib%{name}.*a

%changelog
