Summary: Extra Modules for the Enlightenment window manager
Name: e_modules
Version: 0.0.1
Release: 0.%(date '+%Y%m%d')
License: BSD
Group: User Interface/Desktops
URL: http://www.enlightenment.org/
Source: ftp://ftp.enlightenment.org/pub/enlightenment/%{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:Michael Jennings <mej@eterm.org>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
BuildRoot: %{_tmppath}/%{name}-%{version}-root
BuildRequires: ecore-devel, evas-devel, esmart-devel
BuildRequires: edje-devel, eet-devel, enlightenment-devel >= 0.16.999
Requires: enlightenment >= 0.16.999
Requires: e_modules-snow, e_modules-flame, e_modules-monitor, e_modules-weather

%description
Virtual package requiring all e_modules for the Enlightenment window
manager.  Also includes documentation.

%package snow
Summary: Snow module for the Enlightenment window manager.
Group: User Interface/Desktops
Requires: enlightenment >= 0.16.999

%description snow
Snow module for the Enlightenment window manager.

%package flame
Summary: Flame module for the Enlightenment window manager.
Group: User Interface/Desktops
Requires: enlightenment >= 0.16.999

%description flame
Flame module for the Enlightenment window manager.

%package weather
Summary: Weather module for the Enlightenment window manager.
Group: User Interface/Desktops
Requires: enlightenment >= 0.16.999

%description weather
Weather module for the Enlightenment window manager.

%package monitor
Summary: Monitor module for the Enlightenment window manager.
Group: User Interface/Desktops
Requires: enlightenment >= 0.16.999

%description monitor
Monitor module for the Enlightenment window manager.

%package rain
Summary: Rain module for the Enlightenment window manager.
Group: User Interface/Desktops
Requires: enlightenment >= 0.16.999

%description rain
Rain module for the Enlightenment window manager.


%prep
%setup -q

%build
%{configure}
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
%doc AUTHORS COPYING COPYING-PLAIN README

%files flame 
%defattr(-, root, root)
%{_libdir}/%{name}/flame/*
%{_libdir}/enlightenment/modules_extra/flame

%files monitor
%defattr(-, root, root)
%{_libdir}/%{name}/monitor/*
%{_libdir}/enlightenment/modules_extra/monitor

%files snow
%defattr(-, root, root)
%{_libdir}/%{name}/snow/*
%{_libdir}/enlightenment/modules_extra/snow

%files weather
%defattr(-, root, root)
%{_libdir}/%{name}/weather/*
%{_libdir}/enlightenment/modules_extra/weather

%files rain
%defattr(-, root, root)
%{_libdir}/%{name}/rain/*
%{_libdir}/enlightenment/modules_extra/rain

%changelog
