%define use_gnome 0

Summary: Enlightenment Epplets
Name: epplets
Version: 0.7
Release: 0.2
Copyright: BSD
Group: User Interface/X
URL: http://www.enlightenment.org
Packager: Michael Jennings <mej@eterm.org>
Vendor: The Enlightenment Development Team <e-develop@enlightenment.org>
Source: ftp://ftp.enlightenment.org/enlightenment/epplets/%{name}-%{version}.tar.gz
BuildRoot: /var/tmp/%{name}-%{version}
Requires: enlightenment >= 0.16.0

%description
Epplets are small, handy Enlightenment applets, similar to "dockapps"
or "applets" for other packages.  The epplets package contains the
base epplet API library and header files, as well as the core set of
epplets, including CPU monitors, clocks, a mail checker, mixers, a
slideshow, a URL grabber, a panel-like toolbar, and more.

%prep
%setup -q

%build
CFLAGS="$RPM_OPT_FLAGS"
export CFLAGS

%if %{use_gnome}
    %{configure} --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir} --enable-fsstd --disable-autorespawn
%else
    %{configure} --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir} --enable-fsstd
%endif

make

%install
make DESTDIR=$RPM_BUILD_ROOT install

%post
%ifos Linux
    /sbin/ldconfig
%endif

%postun
%ifos Linux
    /sbin/ldconfig
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc ChangeLog
%{_prefix}/include/*
%{_prefix}/lib/*
%{_bindir}/*
%{_datadir}/enlightenment/epplet_icons/*
%{_datadir}/enlightenment/epplet_data/*
