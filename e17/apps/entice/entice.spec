Summary: entice
Name: entice
Version: 0.9.3.004
Release: 0.20050908
License: BSD
Group: Applications/Multimedia
URL: http://www.enlightenment.org/
Source: %{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:Michael Jennings <mej@eterm.org>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
#BuildSuggests: xorg-x11-devel
BuildRequires: libjpeg-devel XFree86-devel
BuildRequires: evas-devel edb-devel imlib2-devel ecore-devel esmart-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
entice

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
%doc AUTHORS ChangeLog COPYING README
%{_bindir}/*
%{_datadir}/*

%changelog
