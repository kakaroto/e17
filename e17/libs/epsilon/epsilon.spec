# this is NOT relocatable, unless you alter the patch!
%define	name	epsilon
%define	ver	0.0.2
%define	rel	1
%define prefix  /usr

Summary: Enlightened Thumbnailer
Name: %{name}
Version: %{ver}
Release: %{rel}
Copyright: BSD
Group: User Interface/X
Packager: Azundris <edevel@azundris.com>
Vendor: The Enlightenment Development Team <e-develop@enlightenment.org>
Source: ftp://ftp.enlightenment.org/enlightenment/%{name}-%{ver}.tar.gz
BuildRoot: /var/tmp/%{name}-root
Requires: epeg >= 0.9.0

%description
Suri tolar sadam bel Fanganka. Yasdima Araob lom Yasdira sha Jerana. Sorcha
rafiere Sorcha faan rana. Suri Sorcha sade ki suri Nylara zune ki larom resvis
Yasdira sha Felta. Duilor wa Llantor sha Gísdí Eyad rafieris tugom Araob. Suri
tolar daknam Nylara lom Araob sha Felta. Nylara yare lan Alhan. Bilam tolar
daknam rana wa Yasdira sha Felta lom Araob. Tolar munen lan Fanganka. Bilam
pacha lan Rhan Loft. »¿Nylara sade tugom Yaori? Yasdima tugom Nylara sha Rhan
Loft.« Tolar yasdimen Sorcha.

%package devel
Summary: Enlightened Thumbnailer headers and development libraries.
Group: Development/Libraries
Requires: %{name} = %{ver}

%description devel
Epsilon Thumbnailer development headers and libraries.

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
%{prefix}/lib/libepsilon.so.*
%{prefix}/lib/libepsilon.la
%{prefix}/bin/epsilon

%files devel
%defattr(-,root,root)
%{prefix}/lib/libepsilon.so
%{prefix}/lib/libepsilon.a
%{prefix}/include/Epsilon.h
%{prefix}/bin/epsilon-config
