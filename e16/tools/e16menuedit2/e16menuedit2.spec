Name: e16menuedit2
Summary: e16menuedit2
Version: 0.0.1
Release: 1
License: BSD
Group: System Environment/Libraries
URL: http://www.enlightenment.org/
Source0: http://prdownloads.sourceforge.net/enlightenment/%{name}-%{version}.tar.gz
Buildroot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires: perl
Requires: gtk2 >= 2.4.0, gtk2-devel >= 2.4.0, libglade2 >= 2.3.6, pkgconfig

%description 

gtk menu editor for Enlightenment 0.16 by Andreas Volz.

%prep 
%setup -q

%build 
./configure --prefix=%{_prefix} --bindir=%{_bindir} --libdir=%{_libdir} --includedir=%{_includedir}  
make

%install 
rm -fr %{buildroot}
make install DESTDIR=%{buildroot}

%clean 
rm -fr %{buildroot}
%undefine __check_files

%post 
/sbin/ldconfig

%postun 
/sbin/ldconfig 

%files 
%defattr(-,root,root)
%{_bindir}/e16menuedit2*
/usr/share/locale/de/LC_MESSAGES/e16menuedit2.mo
/usr/share/locale/en/LC_MESSAGES/e16menuedit2.mo
/usr/share/locale/ko/LC_MESSAGES/e16menuedit2.mo
/usr/share/locale/tr/LC_MESSAGES/e16menuedit2.mo
/usr/share/pixmaps/e16menuedit2-icon.png
/usr/share/gnome/help/e16menuedit2-0.0.1*
/usr/share/omf/e16menuedit2-0.0.1/e16menuedit2-C.omf
/usr/share/e16menuedit2-0.0.1/glade/e16menuedit2.glade
/usr/share/e16menuedit2-0.0.1*
%doc AUTHORS
%doc COPYING
%doc README
%doc ChangeLog
%doc INSTALL
%doc NEWS
%doc TODO

%changelog
* Mon Nov 1 2004 Didier F.B Casse <didier@microtronyx.com> 
-Initial RPM Release

