# this is NOT relocatable, unless you alter the patch!
%define	name	ewd
%define	ver	0.0.1
%define	rel	1
%define prefix  /usr

Summary: Enlightened Widget Library
Name: %{name}
Version: %{ver}
Release: %{rel}
Copyright: BSD
Group: User Interface/X
URL: http://www.enlightenment.org/efm.html
Packager: The Rasterman <raster@rasterman.com> Term <term@twistedpath.org>
Vendor: The Enlightenment Development Team <e-develop@enlightenment.org>
Source: ftp://ftp.enlightenment.org/enlightenment/%{name}-%{ver}.tar.gz
BuildRoot: /var/tmp/%{name}-root

%description
Ewd is brub.

%package devel
Summary: Ewd headers and development libraries.
Group: Development/Libraries
Requires: %{name} = %{ver}

%description devel
Ewd development files

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
%{prefix}/lib/libewd.so.*

%files devel
%defattr(-,root,root)
%{prefix}/lib/libewd.so
%{prefix}/lib/libewd.*a
%{prefix}/include/Ewd.h
%{prefix}/bin/ewd-config
