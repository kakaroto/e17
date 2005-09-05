%define _missing_doc_files_terminate_build 0

Summary: EFL-based Terminal
Name: enterminus
Version: 0.1
Release: 1
License: BSD
Group: User Interface/X
URL: http://www.enlightenment.org/pages/enterminus.html
Source: ftp://ftp.enlightenment.org/enlightenment/%{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:Michael Jennings <mej@eterm.org>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
#BuildSuggests: xorg-x11-devel
BuildRequires: libjpeg-devel XFree86-devel
BuildRequires: evas-devel ecore-devel edje-devel esmart-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
Enterminus is an EFL-based terminal emulator smart object for evas.

%package devel
Summary: Enterminus headers and development libraries.
Group: Development/Libraries
Requires: %{name} = %{version}
Requires: evas-devel edje-devel ecore-devel

%description devel
Enterminus development files

%prep
%setup -q

%build
%{configure} --prefix=%{_prefix}
%{__make} %{?_smp_mflags} %{?mflags}

%install
%{__make} %{?mflags_install} DESTDIR=$RPM_BUILD_ROOT install
test -x `which doxygen` && sh gendoc || :

%post
/sbin/ldconfig || :

%postun
/sbin/ldconfig || :

%clean
test "x$RPM_BUILD_ROOT" != "x/" && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING* NEWS README TODO
%{_bindir}/%{name}
%{_datadir}/%{name}

%files devel
%defattr(-, root, root)
%doc doc/html
%{_bindir}/%{name}-config

%changelog
