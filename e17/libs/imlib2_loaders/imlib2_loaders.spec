%define	name	 imlib2_loaders
%define ver      0.0.1
%define rel      1
%define prefix   /usr

Summary: Additional image loaders for Imlib2
Name: %{name}
Version: %{ver}
Release: %{rel}
Copyright: BSD
Group: System/Libraries
Source: ftp://ftp.enlightenment.org/pub/enlightenment/e17/libs/%{name}-%{ver}.tar.gz
BuildRoot: %{_tmppath}/%{name}-root
Packager: Christian Kreibich <cK@whoop.org>
URL: http://www.rasterman.com/raster/imlib.html
Requires: imlib2
%description
This package contains additional image loaders for Imlib2,
which for some reason (such as license issues) are not
distributed with Imlib2 directly.

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
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%attr(755,root,root) %{_libdir}/loaders/filter/*
%changelog
* Tue May 22 2001 Christian Kreibich <cK@whoop.org>
- Initial spec file
