Summary: EFL enabled iconbar
Name: iconbar
Version: 0.9.1
Release: 2.%(date '+%Y%m%d')
Copyright: BSD
Group:System Environment/Libraries
Source:ftp://ftp.enlightenment.org/enlightenment/e17/apps/%{name}-%{version}.tar.gz
BuildRequires: imlib2 eet evas edje esmart
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
This is a standalone iconbar based on the e17 iconbar by raster and rephorm.
As of v0.5 the iconbar now uses Edje for both its theme and its icons. This
allows for all sorts of animation and other effects. As of now, the included
themes mimic the old iconbar behavior. Expect to see themes taking advantage
of Edje's capabilities in the future.

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q

%build
if [ -e ./configure ]
  then
    %{configure} --prefix=%{_prefix} --bindir=%{_bindir} --libdir=%{_libdir} --includedir=%{_includedir} --datadir=%{_datadir}
  else
    ./autogen.sh --prefix=%{_prefix} --bindir=%{_bindir} --libdir=%{_libdir} --includedir=%{_includedir} --datadir=%{_datadir}
fi
%{__make} %{?_smp_mflags} %{?mflags}

%install
%{__make} %{?mflags_install} DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc AUTHORS COPYING INSTALL README TODO ChangeLog
%{_bindir}/iconbar
%{_datadir}/*
%changelog
* Fri Aug 27 2004 Evolution <jperrin@gmail.com>
- Initial spec file creation.
