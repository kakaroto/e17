Summary: engage
Name: engage
Version: 0.0.9
Release: 1.%(date '+%Y%m%d')
License: BSD
Group: Applications/Multimedia
URL: http://www.enlightenment.org/
Source: %{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:Michael Jennings <mej@eterm.org>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
#BuildSuggests: xorg-x11-devel
BuildRequires: libjpeg-devel XFree86-devel
BuildRequires: evas-devel edje-devel imlib2-devel ecore-devel esmart-devel
BuildRequires: ewl-devel enlightenment-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
engage

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
%{_bindir}/%{name}
%{_libdir}/enlightenment/modules/%{name}*

%changelog
