Summary: Library of Assorted Spiffy Things
Name: libast
Version: 0.6
Release: 0.6
Copyright: BSD
Group: System Environment/Libraries
Source: %{name}-%{version}.tar.gz
URL: http://www.eterm.org/
BuildRoot: /var/tmp/%{name}-root

%description
LibAST is the Library of Assorted Spiffy Things.  It contains various
handy routines and drop-in substitutes for some good-but-non-portable
functions.  It currently has a built-in memory tracking subsystem as
well as some debugging aids and other similar tools.

It's not documented yet, mostly because it's not finished.  Hence the
version number that begins with 0.

%changelog

%prep
%setup -q

%build
CFLAGS="$RPM_OPT_FLAGS"
export CFLAGS
if [ -e ./configure ]; then
  %configure --prefix=%{_prefix} --bindir=%{_bindir} --libdir=%{_libdir} --includedir=%{_includedir} --datadir=%{_datadir}
else
  ./autogen.sh --prefix=%{_prefix} --bindir=%{_bindir} --libdir=%{_libdir} --includedir=%{_includedir} --datadir=%{_datadir}
fi
make

%install
make DESTDIR=$RPM_BUILD_ROOT install

%post
test -x /sbin/ldconfig && /sbin/ldconfig || :

%postun
test -x /sbin/ldconfig && /sbin/ldconfig || :

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc ChangeLog DESIGN README
%{_bindir}/*
%{_libdir}/*
%{_includedir}/*
%{_datadir}/*
