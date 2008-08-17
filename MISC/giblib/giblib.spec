# this is NOT relocatable
%define	name	giblib
%define	ver	1.2.1
%define	rel	1
%define prefix  /usr

Summary: Useful data structure and function support library.
Name: %{name}
Version: %{ver}
Release: %{rel}
Copyright: BSD
Group: User Interface/X
URL: http://linuxbrit.co.uk/
Packager: Term <term@twistedpath.org>
Vendor: The Enlightenment Development Team <e-develop@enlightenment.org>
Source: ftp://ftp.enlightenment.org/enlightenment/%{name}-%{ver}.tar.gz
BuildRoot: /var/tmp/%{name}-root
Requires: imlib2 >= 1.0.0

%description
Giblib is a library with some similiarities to Glib, except that it does
not abstract the entire operating system. What it does provide above Glib
is an Imlib2 abstraction, and several other useful features that make
programming X-based programs easier.

Install giblib if you want to develop applications that need some data
structures and functional support, but don't want to use Glib, or
implement them yourself.

%package devel
Summary: Giblib library headers and development libraries.
Group: Development/Libraries
Requires: %{name} = %{ver}

%description devel
Giblib library headers and development libraries.

%prep
%setup -q

%build
if [ ! -e ./configure ]
then
  ./autogen.sh --prefix=%{prefix}
else
  ./configure --prefix=%{prefix}
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
%doc README AUTHORS ChangeLog TODO
%defattr(-,root,root)
%{prefix}/lib/libgiblib.so.*

%files devel
%defattr(-,root,root)
%{prefix}/lib/libgiblib.so
%{prefix}/lib/libgiblib.la
%{prefix}/lib/libgiblib.a
%{prefix}/include/giblib/*
%{prefix}/bin/giblib-config
