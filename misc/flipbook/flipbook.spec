# Note that this is NOT a relocatable package
%define ver      0.9
%define rel      1
%define prefix   /usr

Summary: VA Flipbook
Name: flipbook
Version: %ver
Release: %rel
Copyright: GPL
Group: User Interface
Source: ftp://ftp.valinux.com/pub/software/flipbook-%{ver}.tar.gz
BuildRoot: /tmp/flipbook-%{ver}-root
Packager: Mandrake <mandrake@mandrake.net>
URL: http://mandrake.net

Docdir: %{prefix}/doc

%description
The VA Flipbook is a an animated still image player, that supports a variety
of image formats, including TIFF, JPEG, PNG, etc.  It will play at any given
framerate, etc etc etc (fill in package information here)

This package will install the VA Flipbook application

%changelog

%prep
%setup

%build
# Optimize that damned code all the way
if [ ! -z "`echo -n ${RPM_OPT_FLAGS} | grep pentium`" ]; then
  if [ ! -z "`which egcs`" ]; then
    CC="egcs" 
  else
    if [ ! -z "`which pgcc`" ]; then
      CC="pgcc"
    fi
  fi
  CFLAGS="${RPM_OPT_FLAGS}"
else
  CFLAGS="${RPM_OPT_FLAGS}"
fi
if [ ! -f configure ]; then
  CFLAGS="$RPM_OPT_FLAGS" ./autogen.sh --prefix=%prefix
else
  CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%prefix
fi
make

%install
rm -rf $RPM_BUILD_ROOT

make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
rm -rf $RPM_BUILD_ROOT

%post

%postun

%files
%defattr(-, root, root)

%{prefix}/bin/*
%{prefix}/share/flipbook/pixmaps/*

%doc AUTHORS
%doc COPYING
%doc INSTALL
%doc README
