Summary: Powerful image loading and rendering library
Name: imlib2
Version: 1.0.5
Release: 1
Copyright: BSD
Group: System Environment/Libraries
Source: ftp://ftp.enlightenment.org/pub/enlightenment/e17/libs/%{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-root
Packager: Lyle Kempler <term@twistedpath.org>, Joakim Bodin <bodin@dreamhosted.com>
URL: http://www.rasterman.com/raster/imlib.html
BuildRequires: libjpeg-devel
BuildRequires: libpng-devel
BuildRequires: XFree86-devel
BuildRequires: freetype-devel

%description
Imlib2 is an advanced replacement library for libraries like libXpm that
provides many more features with much greater flexibility and speed than
standard libraries, including font rasterization, rotation, RGBA space
rendering and blending, dynamic binary filters, scripting, and more.

%package devel
Summary: Imlib2 headers, static libraries and documentation
Group: System Environment/Libraries
Requires: %{name} = %{version}
%description devel
Headers, static libraries and documentation for Imlib2.

%package filters
Summary: Imlib2 basic plugin filters set
Group: System Environment/Libraries
Requires: %{name} = %{version}
%description filters
Basic set of plugin filters that come with Imlib2

%package loader_jpeg
Summary: Imlib2 JPEG loader
Group: System Environment/Libraries
BuildRequires: libjpeg-devel
%description loader_jpeg
JPEG image loader/saver for Imlib2

%package loader_png
Summary: Imlib2 PNG loader
Group: System Environment/Libraries
BuildRequires: libpng-devel
BuildRequires: zlib-devel
%description loader_png
PNG image loader/saver for Imlib2

%package loader_argb
Summary: Imlib2 ARGB loader
Group: System Environment/Libraries
%description loader_argb
ARGB image loader/saver for Imlib2

%package loader_bmp
Summary: Imlib2 BMP loader
Group: System Environment/Libraries
%description loader_bmp
BMP image loader/saver for Imlib2

%package loader_gif
Summary: Imlib2 GIF loader
Group: System Environment/Libraries
%description loader_gif
GIF image loader for Imlib2

%package loader_pnm
Summary: Imlib2 PNM loader
Group: System Environment/Libraries
%description loader_pnm
PNM image loader/saver for Imlib2

%package loader_tga
Summary: Imlib2 TGA loader
Group: System Environment/Libraries
%description loader_tga
TGA image loader/saver for Imlib2

%package loader_tiff
Summary: Imlib2 TIFF loader
Group: System Environment/Libraries
BuildRequires: libtiff-devel
%description loader_tiff
TIFF image loader/saver for Imlib2

%package loader_xpm
Summary: Imlib2 XPM loader
Group: System Environment/Libraries
%description loader_xpm
XPM image loader/saver for Imlib2

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q

%build

if [ -f configure ]
then
  %configure --prefix=%{_prefix} --bindir=%{_bindir} --libdir=%{_libdir} --includedir=%{_includedir}
else
  # can't build as root? gah. ;)
  sed <autogen.sh 's@$USER@ANNOYING@' >autogen.sh1
  mv -f autogen.sh1 autogen.sh
  chmod 0700 autogen.sh
  ./autogen.sh --prefix=%{_prefix} --bindir=%{_bindir} --libdir=%{_libdir} --includedir=%{_includedir}
fi

if [ "$SMP" != "" ]; then
  (make "MAKE=make -k -j $SMP"; exit 0)
  make
else
  make
fi

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%post
%ifos linux
/sbin/ldconfig
%endif

%postun
%ifos linux
/sbin/ldconfig
%endif

%files
%defattr(-,root,root,0755)
%doc AUTHORS README COPYING ChangeLog doc/index.html doc/imlib2.gif doc/blank.gif
%{_libdir}/lib*.so.*
%{_bindir}/*

%files devel
%defattr(-,root,root,0755)
%{_libdir}/libImlib2.so
%{_libdir}/*.a
%{_includedir}/*

%files filters
%attr(755,root,root) %{_libdir}/loaders/filter/*

%files loader_jpeg
%attr(755,root,root) %{_libdir}/loaders/image/jpeg.*

%files loader_png
%attr(755,root,root) %{_libdir}/loaders/image/png.*

%files loader_argb
%attr(755,root,root) %{_libdir}/loaders/image/argb.*

%files loader_bmp
%attr(755,root,root) %{_libdir}/loaders/image/bmp.*

%files loader_gif
%attr(755,root,root) %{_libdir}/loaders/image/gif.*

%files loader_pnm
%attr(755,root,root) %{_libdir}/loaders/image/pnm.*

%files loader_tga
%attr(755,root,root) %{_libdir}/loaders/image/tga.*

%files loader_tiff
%attr(755,root,root) %{_libdir}/loaders/image/tiff.*

%files loader_xpm
%attr(755,root,root) %{_libdir}/loaders/image/xpm.*

%changelog
* Mon Jan 8 2001 The Rasterman <raster@rasterman.com>
- Fix Requires & BuildRequires for freetype.

* Sat Sep 30 2000 Lyle Kempler <term@kempler.net>
- Bring back building imlib2 as root via autogen.sh for the lazy (me)
- Some minor changes

* Sat Sep 30 2000 Joakim Bodin <bodin@dreamhosted.com>
- Linux-Mandrake:ise the spec file

* Tue Sep 12 2000 The Rasterman <raster@rasterman.com>
- Redo spec file

* Wed Aug 30 2000 Lyle Kempler <kempler@utdallas.edu>
- Include imlib2-config

* Sat May 20 2000 Lyle Kempler <kempler@utdallas.edu>
- Fixed problems with requiring imlib2_view
- Went back to imlib2_view (not imlib2-view)

* Tue Nov 2 1999 Lyle Kempler <kempler@utdallas.edu>
- Mangled imlib 1.9.8 imlib spec file into imlib2 spec file
