# this is NOT relocatable, unless you alter the patch!
%define	name	ewl
%define	ver	0.0.0
%define	rel	1
%define prefix  /usr

Summary: Enlightenment Widget Library
Name: %{name}
Version: %{ver}
Release: %{rel}
Copyright: BSD
Group: User Interface/X
URL: http://www.enlightenment.org/pages/ewl.html
Packager: The Rasterman <raster@rasterman.com> Term <term@kempler.net>
Vendor: The Enlightenment Development Team <e-develop@enlightenment.org>
Source: ftp://ftp.enlightenment.org/enlightenment/%{name}-%{ver}.tar.gz
BuildRoot: /var/tmp/%{name}-root

%description
Ewl is brub.

%package devel
Summary: Ewl headers and development libraries.
Group: Development/Libraries
Requires: %{name} = %{ver}

%description devel
Ewl development files

%prep
%setup -q

%build
if [ -e ./configure ]
then
  ./configure --prefix=%{prefix}
else
  ./autogen.sh --prefix=%{prefix}
fi
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
%{prefix}/lib/libewl.so.*

%files devel
%defattr(-,root,root)
%{prefix}/lib/libewl.so
%{prefix}/lib/libewl.*a
%{prefix}/include/Ewl.h
%{prefix}/include/ewl_*.h
%{prefix}/bin/ewl-config
