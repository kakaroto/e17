Summary: Evas "smart objects"
Name: esmart
Version: 0.9.0
Release: 1
Copyright: BSD
Group: User Interface/X
Source: ftp://ftp.enlightenment.org/enlightenment/%{name}-%{version}.tar.gz
URL: http://www.enlightenment.org/pages/efl.html
Packager: Michael Jennings <mej@eterm.org>
Vendor: The Enlightenment Development Team <e-develop@enlightenment.org>
Requires: evas >= 1.0.0
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
Esmart contains "smart" pre-built evas objects.  It currently includes
a thumbnail generator and a horizontal/vertical container.

%package devel
Summary: Eves "smart objects" headers and development libraries.
Group: Development/Libraries
Requires: %{name} = %{version}

%description devel
Evas "smart objects" development headers and libraries.

%prep
%setup -q

%build
%{configure} --prefix=%{_prefix}
%{__make} %{?_smp_mflags} %{?mflags}

%install
%{__make} %{?mflags_install} DESTDIR=$RPM_BUILD_ROOT install
#test -x `which doxygen` && sh gendoc || :

%post
/sbin/ldconfig || :

%postun
/sbin/ldconfig || :

%clean
test "x$RPM_BUILD_ROOT" != "x/" && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING* README
%{_libdir}/libesmart_*.so.*
%{_libdir}/libesmart_*.la
%{_libdir}/esmart/layout/*.so
%{_libdir}/esmart/layout/*.la
%{_bindir}/esmart_file_dialog_test
%{_bindir}/esmart_test
%{_datadir}/esmart/esmart.png

%files devel
%defattr(-, root, root)
#%doc doc/html
%{_libdir}/libesmart_*.so
%{_libdir}/libesmart_*.a
%{_libdir}/esmart/layout/*.a
%{_includedir}/Esmart/Esmart_*
%{_bindir}/esmart-config
%{_libdir}/pkgconfig/esmart.pc

%changelog
