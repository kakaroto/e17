%define	name	 gevas
%define ver      2.2.1
%define rel      monkeyiq9000d
%define prefix   /usr

%define prefix      %{_prefix}
%define sysconfdir  /etc
%define exec_prefix %{_prefix}
%define bindir      %{_bindir}
%define libdir      %{_libdir}
%define includedir  %{_includedir}
%define buildroot   /tmp/build/%{name}-root

Summary: GTK+ wrapper for evas.
Name: %{name}
Version: %{ver}
Release: %{rel}
License: LGPL
Group: System/Libraries
Source: http://216.136.171.200/enlightenment/%{name}-%{ver}.tar.gz
BuildRoot: %{buildroot}
Packager: Ben Martin <monkeyiq@dingoblue.net.au>
URL: http://sourceforge.net/project/showfiles.php?group_id=2
Requires: evas >= 0.9.9.013
BuildRequires: evas-devel >= 0.9.9.013

%description
gevas is a GTK+ wrapper for the Evas API. 

Gtk+ is the GIMP ToolKit (GTK+).

Evas is an advanced canvas library, providing three backends for
rendering: X11 (without some features like alpha-blending), imlib2, or
OpenGL (hardware accelerated). 

%package devel
Summary: gevas headers and documentation
Group: System/Libraries
Requires: %{name} = %{PACKAGE_VERSION}
%description devel
Headers and documentation for gevas.


%prep
rm -rf $RPM_BUILD_ROOT

%setup -q

%build

if [ -f configure ]
then
  %configure
#./configure --prefix=%prefix
else
  # can't build as root? gah. ;)
  sed <autogen.sh 's@$USER@ANNOYING@' >autogen.sh1
  mv -f autogen.sh1 autogen.sh
  chmod 0700 autogen.sh
  ./autogen.sh --prefix=%prefix
fi

if [ "$SMP" != "" ]; then
  (make "MAKE=make -k -j $SMP"; exit 0)
  make
else
  make
fi

%install
rm -rf $RPM_BUILD_ROOT
make \
	DESTDIR="$RPM_BUILD_ROOT" \
	RPM_BUILD_ROOT="$RPM_BUILD_ROOT" \
	install

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,0755)
#%doc AUTHORS README COPYING ChangeLog 
%{_libdir}/lib*.so*
%{_bindir}/*
%{_datadir}/*

%files devel
%defattr(-,root,root,0755)
%{_libdir}/*.a
%{_libdir}/*.la
%{_includedir}/*


%changelog
* Fri Mar 30 2001 Ben Martin
- Created 
