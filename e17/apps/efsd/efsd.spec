Summary: Enlightenment File System Daemon and library
Name: efsd
Version: 0.0.1
Release: 1.%(date '+%Y%m%d')
License: BSD
Group: System Environment/Libraries
URL: http://www.enlightenment.org/
Source: ftp://ftp.enlightenment.org/pub/efsd/%{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:Michael Jennings <mej@eterm.org>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
BuildRequires: edb-devel
Requires: fam
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
Efsd is a file system daemon that provides a facility to launch
filesystem commands such as cp, rm, mv, stat etc, get file type
information, get and set arbitrary file metadata, receive file
modification events and more in an asynchronous manner.

%package devel
Summary: Enlightenment File System Daemon library, and headers.
Group: System Environment/Daemons
Requires: %{name} = %{version}
Requires: edb-devel

%description devel
Efsd development headers and library.

%prep
%setup -q

%build
%{configure} --prefix=%{_prefix}
%{__make} %{?_smp_mflags} %{?mflags}

%install
%{__make} %{?mflags_install} DESTDIR=$RPM_BUILD_ROOT install

%clean
test "x$RPM_BUILD_ROOT" != "x/" && rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(-, root, root)
%doc AUTHORS ChangeLog COPYING README TODO
%{_bindir}/%{name}
%{_bindir}/%{name}sh
%{_libdir}/*
%{_datadir}/%{name}/filetypes*

%files devel
%defattr(-, root, root)
%{_bindir}/%{name}-config
%{_includedir}/*
%{_datadir}/%{name}/gdb.scr

%changelog
