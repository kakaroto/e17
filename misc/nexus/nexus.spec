# Note that this is NOT a relocatable package
%define ver      0.0.2
%define rel      1

Summary: Nexus Font for X11
Name: xfont-nexus
Version: %ver
Release: %rel
Copyright: BSD
Group: User Interface/X
Source: ftp://ftp.vergenet.net/pub/nexus/nexus-0.0.2.tar.gz
BuildRoot: /tmp/xfont-nexus-root
Packager: Horms <horms@vergenet.net>
URL: ftp://ftp.vergenet.net/pub/nexus/
Requires: XFree86-xfs
Docdir: /usr/doc

%description
Nexus font for X11

%prep
%setup -n nexus-%{ver}

%build

%install
rm -rf $RPM_BUILD_ROOT

DESTDIR="$RPM_BUILD_ROOT" make install
mkdir -p $RPM_BUILD_ROOT/usr/X11R6/lib/X11/fonts/misc
install -c -m 644 nex6x10.pcf $RPM_BUILD_ROOT/usr/X11R6/lib/X11/fonts/misc
gzip --best $RPM_BUILD_ROOT/usr/X11R6/lib/X11/fonts/misc/nex6x10.pcf

%clean
rm -rf $RPM_BUILD_ROOT

%post
( cd /usr/X11R6/lib/X11/fonts/misc ;  /usr/X11R6/bin/mkfontdir  )

%postun
test -d /usr/X11R6/lib/X11/fonts/misc && ( cd /usr/X11R6/lib/X11/fonts/misc ;  /usr/X11R6/bin/mkfontdir  )

%files
%defattr(-, root, root)
/usr/X11R6/lib/X11/fonts/misc/nex6x10.pcf.gz
%dir /usr/X11R6/lib/X11/fonts/misc/
