%define	name	efsd
%define	ver	0.0.1
%define	rel	1
%define prefix  /usr

Summary: Enlightenment File System Daemon and library
Name: %{name}
Version: %{ver}
Release: %{rel}
Copyright: BSD
Group: System Environment/Libraries
URL: http://whoop.org/enlightenment.html
Packager: Christian Kreibich <cK@whoop.org>
Vendor: The Enlightenment Development Team <e-develop@enlightenment.org>
Source: ftp://ftp.enlightenment.org/enlightenment/%{name}-%{ver}.tar.gz
BuildRoot: /var/tmp/%{name}-root
Requires: edb-devel >= 1.0.2
Requires: fam

%description
Efsd is a file system daemon that provides a facility to launch filesystem
commands such as cp, rm, mv, stat etc, get file type information, get and
set arbitrary file metadata, receive file modification events and more in
an asynchronous manner.

%package devel
Summary: Enlightenment File System Daemon, library, and headers.
Group: System Environment/Daemons
Requires: %{name} = %{ver}

%description devel
Efsd development headers and library.

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
%{prefix}/bin/efsd
%{prefix}/bin/efsdsh
%{prefix}/bin/efsd-config
%{prefix}/lib/libefsd.*
%{prefix}/share/efsd/magic.db
%{prefix}/share/efsd/pattern.db

%doc AUTHORS
%doc COPYING
%doc README
%doc TODO

%files devel
%defattr(-,root,root)
%{prefix}/bin/efsd
%{prefix}/bin/efsdsh
%{prefix}/bin/efsd-config
%{prefix}/lib/libefsd.*
%{prefix}/share/efsd/magic.db
%{prefix}/share/efsd/pattern.db
%{prefix}/include/libefsd.h
%{prefix}/include/efsd.h

%changelog
* Wed May 16 2001 Christian Kreibich <cK@whoop.org>
- Initial spec file.
