%define name	edox-data
%define version 0.16.7
%define release 0.02

Summary:	Documentation for the Enlightenment window manager.
Name:		%{name}
Version:	%{version}
Release:	%{release}
Copyright:	BSD
Group:		User Interface/Desktops
Source:		%{name}-%{version}.tar.gz
Prefix:		%{_prefix}
Docdir:		%{_docdir}
BuildArch:	noarch
BuildRoot:	/tmp/build-%{name}-%{version}-root
Packager:	Michael Jennings <mej@eterm.org>
URL:		http://www.enlightenment.org/

Requires: enlightenment >= 0.16.7

%description
These are the DOX data files.
DOX is a very very very simple document viewer for help docs for Enlightenment.
This is part of the Enlightenment distribution.

%changelog

%prep
%setup

%build
CFLAGS="${RPM_OPT_FLAGS}"
if [ ! -f configure ]; then
  ./autogen.sh --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir} --mandir=%{_mandir} --enable-fsstd
else
  %{configure} --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir} --mandir=%{_mandir} --enable-fsstd
fi
make

%install
rm -rf $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING INSTALL README
%{_datadir}/enlightenment/*
