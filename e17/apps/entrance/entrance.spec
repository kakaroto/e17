Summary: enlightened display manager
Name: entrance
Version: 0.9.0.004
Release: 1
License: BSD
Group: System Environment/Libraries
URL: http://www.enlightenment.org/
Source: ftp://ftp.enlightenment.org/pub/entrance/%{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:Michael Jennings <mej@eterm.org>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
BuildRequires: libjpeg-devel zlib-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
Entrance is the Enlightenment Display Manager. And like Enlightenment,
it takes beauty and customization to levels that KDM and GDM can only
dream about...and without the bloat.

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
%{_sysconfdir}/entrance_config.cfg
%{_sysconfdir}/init.d/entrance
%{_sysconfdir}/pam.d/entrance
%{_bindir}/entrance
%{_bindir}/entrance_edit
%{_bindir}/entrance_login
%{_bindir}/entrance_wrapper
%{_sbindir}/entranced
%{_datadir}/%{name}

%changelog
