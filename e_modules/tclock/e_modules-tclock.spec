%define module_name tclock

Summary: %{module_name} module for the Enlightenment window manager
Name: e_modules-%{module_name}
Version: 0.0.2
Release: 0.%(date '+%Y%m%d')
License: BSD
Group: User Interface/Desktops
URL: http://www.enlightenment.org/
Source: ftp://ftp.enlightenment.org/pub/enlightenment/%{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:Michael Jennings <mej@eterm.org>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
BuildRequires: ecore-devel, evas-devel, esmart-devel
BuildRequires: edje-devel, eet-devel, enlightenment-devel >= 0.16.999
Requires: enlightenment >= 0.16.999
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
%{module_name} module for the Enlightenment window manager.

%prep
%setup -q

%build
%{configure}
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
%doc AUTHORS ChangeLog COPYING COPYING-PLAIN INSTALL NEWS README
%{_libdir}/e_modules/%{module_name}/*
%{_libdir}/enlightenment/modules_extra/%{module_name}

%changelog
