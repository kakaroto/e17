# Note that this is NOT a relocatable package
%define ver      0.0.4
%define rel      1
%define prefix   /usr

Summary: Powerful image loading and rendering library for X11R6
Name: imlib2
Version: %ver
Release: %rel
Copyright: LGPL
Group: X11/Libraries
Source: ftp://ftp.enlightenment.org/pub/enlightenment/libs/imlib2-%{ver}.tar.gz
BuildRoot: /var/tmp/imlib-root
Packager: Term <kempler@utdallas.edu>
URL: http://www.rasterman.com/raster/imlib.html
Requires: freetype >= 1.1
Requires: libpng 
Requires: libjpeg
Requires: zlib 

Docdir: %{prefix}/doc

%description
Imlib2 is an advanced replacement library for libraries like libXpm that
provides many more features with much greater flexability and speed than
standard libraries, including font rasterization, rotation, RGBA space
rendering and blending, and more.

%package devel
Summary: Imlib2 headers, static libraries and documentation
Group: X11/Libraries
Requires: imlib2 = %{PACKAGE_VERSION}

%description devel
Headers, static libraries and documentation for Imlib2.

#### dont build demos 
#%package demos
#Summary: Imlib2 demo programs
#Group: X11/Libraries
#Requires: imlib2 = %{PACKAGE_VERSION}
#
#%description demos
#Demonstration viewer and other programs.

%prep
%setup

%build
##### Boring normal rpm build method
# old method of building - does not auto-detect for options in OPT_FLAGS
###########################################################################
#CFLAGS="${RPM_OPT_FLAGS}" ./configure --prefix=%prefix
#make
###########################################################################

##### High-perfromance auto-detect
# Needed for snapshot releases.
# Optimize that damned code all the way
###########################################################################
#if [ ! -z "echo -n ${RPM_OPT_FLAGS} | grep pentium" ]; then
#  if [ -z "${CC}" -a ! -z "`which egcs`" ]; then
#    CC="egcs"
#    export CC
#  else
#    if [ -z "${CC}" -a ! -z "`which pgcc`" ]; then
#      CC="pgcc"
#      export CC
#    fi
#  fi
#  CFLAGS="${RPM_OPT_FLAGS}"
#else
#  CFLAGS="${RPM_OPT_FLAGS}"
#fi
#export CFLAGS
#
if [ ! -f configure ]; then
  sed <autogen.sh 's@$USER@ANNOYING@' >autogen.sh1
  mv -f autogen.sh1 autogen.sh
  chmod 0700 autogen.sh
  ./autogen.sh --prefix=%prefix
else
  ./configure --prefix=%prefix
fi

if [ "$SMP" != "" ]; then
  (make "MAKE=make -k -j $SMP"; exit 0)
  make
else
  make
fi

## added to make the viewer
cd demo
make
mv imlib2_view imlib2-view
###########################################################################

%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT%{prefix} install
mkdir -p $RPM_BUILD_ROOT%{prefix}/bin
cd demo
cp imlib2-view $RPM_BUILD_ROOT%{prefix}/bin

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%doc README COPYING ChangeLog TODO
%attr(755,root,root) %{prefix}/lib/lib*.so.*
%attr(755,root,root) %{prefix}/lib/loaders

%files devel
%defattr(-,root,root)
%{prefix}/lib/libImlib2.so
%{prefix}/lib/*a
%{prefix}/include/*

#%files demos
#%defattr(-,root,root)
#%{prefix}/bin/imlib2-view

%changelog
* Tue Nov 2 1999 Lyle Kempler <kempler@utdallas.edu>
- Mangled imlib 1.9.8 imlib spec file into imlib2 spec file
