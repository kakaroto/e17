Summary: Additional image loaders for Imlib2
Name: imlib2_loaders
Version: 1.2.0
Release: 2.%(date '+%Y%m%d')
License: Mixed
Group: System Environment/Libraries
URL: http://www.enlightenment.org/pages/imlib2.html
Source: ftp://ftp.enlightenment.org/pub/enlightenment/e17/libs/%{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:Michael Jennings <mej@eterm.org>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
#BuildSuggests: xorg-x11-devel
BuildRequires: freetype-devel XFree86-devel imlib2-devel eet-devel edb-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
This package contains additional image loaders for Imlib2 which for
some reason (such as license issues) are not distributed with Imlib2
directly.

%prep
%setup -q

%build
%{configure} --prefix=%{_prefix}
%{__make} %{?_smp_mflags} %{?mflags}

%install
%{__make} %{?mflags_install} DESTDIR=$RPM_BUILD_ROOT install

%clean
test "x$RPM_BUILD_ROOT" != "x/" && rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(-, root, root)
%{_libdir}/imlib2/loaders/*

%changelog
