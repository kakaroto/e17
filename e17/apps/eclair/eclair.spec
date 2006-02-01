Summary: The Eclair window manager
Name: eclair
Version: 0.0.1
Release: 0.20050908
License: BSD
Group: User Interface/Desktops
URL: http://www.enlightenment.org/
Source: ftp://ftp.enlightenment.org/pub/eclair/%{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:Michael Jennings <mej@eterm.org>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
Prefix: %{_prefix}
#BuildSuggests: xorg-x11-devel
BuildRequires: sqlite3-devel libxml2-devel taglib-devel
BuildRequires: libjpeg-devel XFree86-devel eet-devel embryo-devel emotion-devel
BuildRequires: evas-devel edb-devel edje-devel imlib2-devel ecore-devel esmart-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
Eclair is a media player.

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
%doc AUTHORS COPYING README
%{_bindir}/*
%{_datadir}/%{name}
