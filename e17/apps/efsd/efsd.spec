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
Requires: edb >= 1.0.2
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
%{prefix}/lib/libefsd.so.*
%{prefix}/share/efsd/filetypes.xml
%{prefix}/share/efsd/filetypes.dtd

%doc AUTHORS
%doc COPYING
%doc README
%doc TODO

%files devel
%defattr(-,root,root)
%{prefix}/bin/efsd-config
%{prefix}/lib/libefsd.so
%{prefix}/lib/libefsd.*a
%{prefix}/include/libefsd.h
%{prefix}/include/efsd.h
%{prefix}/share/efsd/gdb.scr

%changelog
* Wed Mar 27 2002 Alastair Tse <altse@cse.unsw.edu.au>
- Replaced edb-devel dependency
- magic.db files are out and filetypes.* files are in
- Moved libefsd.* around
* Wed May 16 2001 Christian Kreibich <cK@whoop.org>
- Initial spec file.
