Summary: enlightened display manager
Name: entrance
Version: 0.9.0
Release: 1.%(date '+%Y%m%d')
Copyright: BSD
Group: System Environment/Libraries
Source: ftp://ftp.enlightenment.org/pub/entrance/%{name}-%{version}.tar.gz
Packager: Michael Jennings <mej@eterm.org>
URL: http://www.enlightenment.org/
#BuildSuggests: libjpeg-devel zlib-devel
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
%{_sysconfdir}/entrance_config.db
%{_sysconfdir}/init.d/entrance
%{_sysconfdir}/pam.d/entrance
%{_bindir}/entrance
%{_bindir}/entrance_edit
%{_bindir}/entrance_login
%{_bindir}/entrance_wrapper
%{_sbindir}/entranced
%{_datadir}/entrance/fonts/*.ttf
%{_datadir}/entrance/images/*.png
%{_datadir}/entrance/images/sessions/*
%{_datadir}/entrance/themes/*.eet
%{_datadir}/entrance/users/*.eet

%changelog
