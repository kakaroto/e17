Name: exhibit
Summary: Yet another image viewer that uses Etk as its toolkit
Version: 0.0.1
Release: 0.%(date '+%Y%m%d')cvs%{?dist}
License: BSD
Group: Applications/Multimedia
URL: http://www.enlightenment.org
Source0: %{name}-%{version}-%(date '+%Y%m%d').tar.gz
Buildroot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: ecore-devel, evas-devel, edje-devel, etk-devel, epsilon-devel
BuildRequires: enlightenment-devel, engrave-devel

%description 
Exhibit is an image viewer that uses Etk as its toolkit. Exhibit supports
image previews for image types supported by Evas and allows for
directory changing using a point and click interface or a text input box with
tab autocompletion support. Exhibit also has image effect, tabs, and favorites
support.

%prep 
%setup -q

%build 
rm -rf missing
touch README
aclocal -I m4
autoheader
autoconf
libtoolize --copy --automake
automake --add-missing --copy --gnu

%configure
%{__make} %{?_smp_mflags}

%install 
rm -fr %{buildroot}
%{__make} %{?mflags_install} DESTDIR=%{buildroot} install

%clean 
rm -fr %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig 

%files 
%defattr(-,root,root)
%{_bindir}/exhibit*
%{_datadir}/exhibit*
%doc AUTHORS ChangeLog COPYING* INSTALL NEWS README TODO

%changelog
* Fri Jan 27 2006 Didier F. B. Casse <didier[AT]microtronyx.com> - 0.0.1-20060127
- Initial RPM release


