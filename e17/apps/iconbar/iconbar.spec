Summary: EFL-enabled iconbar
Name: iconbar
Version: 0.9.1
Release: 0.20050908
License: BSD
Group:System Environment/Libraries
Source:ftp://ftp.enlightenment.org/enlightenment/e17/apps/%{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:Michael Jennings <mej@eterm.org>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
BuildRequires: imlib2-devel eet-devel evas-devel edje-devel esmart-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
This is a standalone iconbar based on the e17 iconbar by raster and
rephorm.  As of v0.5 the iconbar now uses Edje for both its theme and
its icons. This allows for all sorts of animation and other
effects. As of now, the included themes mimic the old iconbar
behavior. Expect to see themes taking advantage of Edje's capabilities
in the future.

%prep
%setup -q

%build
%{configure} --prefix=%{_prefix}
%{__make} %{?_smp_mflags} %{?mflags}

%install
%{__make} %{?mflags_install} DESTDIR=$RPM_BUILD_ROOT install

%clean
test "x$RPM_BUILD_ROOT" != "x/" && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING INSTALL README TODO ChangeLog
%{_bindir}/%{name}
%{_datadir}/*

%changelog
