Summary: Additional image loaders for Imlib2
Name: imlib2_loaders
Version: 1.2.0
Release: 1.%(date '+%Y%m%d')
Copyright: Mixed
Group: System Environment/Libraries
Source: ftp://ftp.enlightenment.org/pub/enlightenment/e17/libs/%{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Packager: Michael Jennings <mej@eterm.org>
URL: http://www.enlightenment.org/pages/imlib2.html
#BuildSuggests: freetype-devel xorg-x11-devel imlib2-devel imlib2

%description
This package contains additional image loaders for Imlib2,
which for some reason (such as license issues) are not
distributed with Imlib2 directly.

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
%{_libdir}/%{name}

%changelog
