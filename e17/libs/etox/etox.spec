# this is NOT relocatable, unless you alter the patch!
%define	name	etox
%define	ver	0.9.0
%define	rel	1
%define prefix  /usr

Summary: Enlightened Text Object Library
Name: %{name}
Version: %{ver}
Release: %{rel}
Copyright: BSD
Group: User Interface/X
URL: http://www.enlightenment.org/efm.html
Packager: Azundris <edevel@azundris.com>
Vendor: The Enlightenment Development Team <e-develop@enlightenment.org>
Source: ftp://ftp.enlightenment.org/enlightenment/%{name}-%{ver}.tar.gz
BuildRoot: /var/tmp/%{name}-root
Requires: evas >= 1.0.0

%description
Suri tolar sadam bel Fanganka. Yasdima Araob lom Yasdira sha Jerana. Sorcha
rafiere Sorcha faan rana. Suri Sorcha sade ki suri Nylara zune ki larom resvis
Yasdira sha Felta. Duilor wa Llantor sha Gísdí Eyad rafieris tugom Araob. Suri
tolar daknam Nylara lom Araob sha Felta. Nylara yare lan Alhan. Bilam tolar
daknam rana wa Yasdira sha Felta lom Araob. Tolar munen lan Fanganka. Bilam
pacha lan Rhan Loft. »¿Nylara sade tugom Yaori? Yasdima tugom Nylara sha Rhan
Loft.« Tolar yasdimen Sorcha.

%package devel
Summary: Enlightened Text Layout Library headers and development libraries.
Group: Development/Libraries
Requires: %{name} = %{ver}

%description devel
Evas development headers and libraries.

%prep
%setup -q

%build
./configure --prefix=%{prefix}
make

%install
make prefix=$RPM_BUILD_ROOT%{prefix} install

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{prefix}/lib/libetox.so.*
%{prefix}/lib/libetox.la
%{prefix}/bin/etox_test
%{prefix}/bin/etox_selections
%{prefix}/share/etox/*

%files devel
%defattr(-,root,root)
%{prefix}/lib/libetox.so
%{prefix}/lib/libetox.a
%{prefix}/include/Etox.h
%{prefix}/bin/etox-config
%{prefix}/share/aclocal/etox.m4
