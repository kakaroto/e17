%define revision 0.00
%define rev_name -%{revision}

Summary: The Enlightenment window manager.
Name: e16
Version: 0.16.8.2
Release: %{revision}%{?_vendorsuffix:.%{_vendorsuffix}}
License: BSD
Group: User Interface/Desktops
URL: http://www.enlightenment.org/
Source0: http://prdownloads.sourceforge.net/enlightenment/%{name}-%{version}%{rev_name}.tar.gz
#BuildSuggests: esound-devel
BuildRequires: imlib2-devel freetype-devel xorg-x11-devel
Packager: %{?_packager:%{_packager}}%{!?_packager:Michael Jennings <mej@eterm.org>}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:The Enlightenment Project (http://www.enlightenment.org/)}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
Prefix: %{_prefix}
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Obsoletes: enlightenment < 0.16.8
Requires: imlib2 >= 1.2.0

%description
Enlightenment is a window manager for the X Window System that
is designed to be powerful, extensible, configurable and
pretty darned good looking! It is one of the more graphically
intense window managers.

Enlightenment goes beyond managing windows by providing a useful
and appealing graphical shell from which to work. It is open
in design and instead of dictating a policy, allows the user to 
define their own policy, down to every last detail.

This package will install the Enlightenment window manager.

%prep
%setup -n %{name}-%{version}%{rev_name}

%build
CFLAGS="${RPM_OPT_FLAGS}"
ENLIGHTENMENT_RELEASE=%{release}
AC_FLAGS="--prefix=%{_prefix} --mandir=%{_mandir} --enable-fsstd"
export CFLAGS ENLIGHTENMENT_RELEASE AC_FLAGS
if [ ! -f configure ]; then
  ./autogen.sh $AC_FLAGS
else
  %{configure} $AC_FLAGS %{?acflags}
fi
%{__make} %{?mflags}

%install
test "x$RPM_BUILD_ROOT" != "x" && rm -rf $RPM_BUILD_ROOT
%{__make} install DESTDIR=$RPM_BUILD_ROOT %{?mflags_install}

%clean
test "x$RPM_BUILD_ROOT" != "x" && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog INSTALL README docs/README-0.16.8
%{_bindir}/*
%{_datadir}/%{name}/*
%{_datadir}/locale/*
%{_datadir}/xsessions/*
#%{_mandir}/*

%changelog
