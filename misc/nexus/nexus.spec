# Note that this is NOT a relocatable package
%define ver      0.0.2
%define rel      1

Summary: Nexus Font for X11
Name: XFree86-nexus-fonts
Version: %ver
Release: %rel
Copyright: BSD
Group: User Interface/X
Source: ftp://ftp.vergenet.net/pub/nexus/xfonts-nexus-0.0.2.tar.gz
BuildRoot: /tmp/XFree86-nexus-fonts-root
Packager: Horms <horms@vergenet.net>
URL: ftp://ftp.vergenet.net/pub/nexus/
Requires: XFree86-xfs
Docdir: /usr/share/doc

%description
Nexus font for X11

%prep
%setup -n xfonts-nexus-%{ver}

%build

%install
rm -rf $RPM_BUILD_ROOT

DESTDIR="$RPM_BUILD_ROOT" make install

%clean
rm -rf $RPM_BUILD_ROOT

%post
( cd /usr/X11R6/lib/X11/fonts/misc ;  \
	/usr/X11R6/bin/mkfontdir;
	cat /usr/share/doc/XFree86-nexus-fonts-%{ver}/nex6x10.alias \
		>> fonts.alias )

%postun
test -d /usr/X11R6/lib/X11/fonts/misc && \
	( cd /usr/X11R6/lib/X11/fonts/misc ;  \
		/usr/X11R6/bin/mkfontdir; 
		ALIAS=$(cat fonts.alias);
		echo "$ALIAS" | grep -v "^nexus -misc-nexus-medium" \
			> fonts.alias;
		chown root.root fonts.alias;
		chmod 444 fonts.alias)

%files
%doc nex6x10.alias AUTHORS COPYING
%defattr(-, root, root)
/usr/X11R6/lib/X11/fonts/misc/nex6x10.pcf.gz
%dir /usr/X11R6/lib/X11/fonts/misc/
