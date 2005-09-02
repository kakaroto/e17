Summary: EFL enabled mail checking utility
Name: embrace
Version: 0.0.3
Release:1.%(date '+%Y%m%d')
License: GPL
Group:User Interface/Desktops
Source: %{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:Jim Perrin <jperrin@gmail.com>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
URL: http://www.enlightenment.org/
BuildRequires: enlightenment >= 0.16.999 evas-devel edje-devel eet-devel edb-devel esmart-devel sylpheed
Requires: enlightenment >= 0.16.999 evas edje eet edb esmart
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
Embrace is a mail-checker which is based on the Enlightenment Foundation Libraries.
It monitors your mailboxes and the current count of total and new mails in them.

%prep
%setup -q

%build
%configure 

%{__make} %{?_smp_mflags} %{?mflags}

%install

%{__make} %{?mflags_install} DESTDIR=$RPM_BUILD_ROOT install
%clean
test "x$RPM_BUILD_ROOT" != "x/" && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog NEWS README INSTALL
%{_libdir}/*
%{_bindir}/*
%{_datadir}/*

%changelog
