Summary: enlightened display manager
Name: entrance
Version: 0.9.0
Release: 1
Copyright: BSD
Group: System Environment/Libraries
Source: ftp://ftp.enlightenment.org/pub/evoak/%{name}-%{version}.tar.gz
Packager: Azundris <edevel@azundris.com>
URL: http://www.enlightenment.org/
BuildRequires: libjpeg-devel zlib-devel
Requires: libjpeg zlib ecore evas eet imlib2
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
entrance is beauteous and highly themable X display manager / login daemon thingie.

%prep
%setup -q

%build
%{configure}

if [ "$SMP" != "" ]; then
  (%{__make} "MAKE=make -k -j $SMP"; exit 0)
  %{__make}
else
  %{__make}
fi

%install
%{__make} DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig || :

%postun
/sbin/ldconfig || :

%files
%defattr(-, root, root)
%doc AUTHORS COPYING README
%{_libdir}/libedje.so*
%{_bindir}/edje
%{_datadir}/edje

%changelog
* Sun May 23 2004 Azundris <edevel@azundris.com>
- Created spec file
