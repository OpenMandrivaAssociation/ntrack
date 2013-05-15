%define major 0
%define libntrack %mklibname ntrack %{major}
%define libntrack_devel %mklibname ntrack -d

%define ntrackqt_major 1
%define libntrackqt %mklibname ntrack-qt4 %ntrackqt_major
%define libntrackqt_devel %mklibname ntrack-qt4 -d 

%define libntrack_gobject_major 1
%define libntrack_gobject %mklibname ntrack-gobject %{libntrack_gobject_major}
%define libntrack_gobject_devel %mklibname ntrack-gobject -d

%define libntrack_glib_major 2
%define libntrack_glib %mklibname ntrack-glib %{libntrack_glib_major}
%define libntrack_glib_devel %mklibname ntrack-glib -d

Name:		ntrack
Summary:	Network Connectivity Tracking library for Desktop Applications
Version:	016
Release:	6
License:	LGPLv3
Url:		https://launchpad.net/%{name}
Source0:	http://launchpad.net/%{name}/main/%{version}/+download/%{name}-%{version}.tar.gz
Patch0:		ntrack-016-relaxed_flags.diff
Patch1:		ntrack-016-automake-1.13.patch
Group:		Development/C
BuildRequires:	autoconf
BuildRequires:	automake
BuildRequires:	libtool
BuildRequires:	pkgconfig(QtCore)
BuildRequires:	libnl-devel
BuildRequires:	python-devel
BuildRequires:	python-gobject-devel
BuildRequires:	libnl3-devel

%description
ntrack aims to be a lightweight and easy to use library for application
developers that want to get events on network online status changes such
as online, offline or route changes.

The primary goal is to serve desktop applications in a network manager 
and desktop environment independent fashion.
Also its supposed to be lightweight, resource un-intensive and extensible.

%package -n %{libntrack}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		System/Libraries

%description -n %{libntrack}
ntrack aims to be a lightweight and easy to use library for application
developers that want to get events on network online status changes such
as online, offline or route changes.

%package -n %{libntrack_devel}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		Development/Other
Provides:	libntrack-devel = %{version}-%{release}

%description -n %{libntrack_devel}
ntrack aims to be a lightweight and easy to use library for application
developers that want to get events on network online status changes such
as online, offline or route changes.

%package -n %{libntrackqt}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		System/Libraries

%description -n %{libntrackqt}
This packages provides the Qt4 bindings for %{name}.

%package -n %{libntrackqt_devel}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		Development/Other
Requires:	%{libntrackqt} = %{version}-%{release}
Provides:	libntrack-qt4-devel = %{version}-%{release}

%description -n %{libntrackqt_devel}
Development Qt4 files (headers and libraries) for ntrack.

%package -n %{libntrack_gobject}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		System/Libraries

%description -n %{libntrack_gobject}
This package provides the gobject bindings for %{name}.

%package -n %{libntrack_gobject_devel}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		Development/Other
Requires:	%{libntrack_gobject} = %{version}-%{release}
Provides:	libntrack-gobject-devel = %{version}-%{release}

%description -n %{libntrack_gobject_devel}
Development GObject files (headers and libraries) for ntrack.

%package -n %{libntrack_glib}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		System/Libraries

%description -n %{libntrack_glib}
This package provides the glib bindings for %{name}.

%package -n %{libntrack_glib_devel}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		Development/Other
Requires:	%{libntrack_glib} = %{version}-%{release}
Provides:	libntrack-glib-devel = %{version}-%{release}

%description -n %{libntrack_glib_devel}
Development files (headers and libraries) for ntrack.

%package devel
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		Development/Other
Requires:	%{libntrack_glib_devel} = %{version}-%{release}
Requires:	%{libntrack_gobject} = %{version}-%{release}
Requires:	%{libntrack_devel} = %{version}-%{release}
Requires:	%{libntrackqt_devel} =  %{version}-%{release}

%description devel
Development files (headers and libraries) for ntrack.

%package -n python-%{name}
Summary:	Python bindings to %{name}
Group:		Development/Python
Requires:	%{libntrack_glib} = %{version}-%{release}
Requires:	%{libntrack_gobject} = %{version}-%{release}

%description -n python-%{name}
Python bindings to %{name}.



%prep
%setup -q
%apply_patches

%build
mkdir -p m4
autoreconf -fi
%configure2_5x
%make

%install
%makeinstall_std

# Remove .a & .la files
rm -rf %{buildroot}/%{_libdir}/*.a
rm -rf %{buildroot}/%{_libdir}/*.la
rm -rf %{buildroot}/%{_libdir}/ntrack/modules/*.a
rm -rf %{buildroot}/%{_libdir}/ntrack/modules/*.la
rm -rf %buildroot%_libdir/python*/site-packages/*.a

# dupes
rm -rf %{buildroot}%{_datadir}/doc/ntrack

%files
%doc README NEWS COPYING COPYING.LESSER ChangeLog AUTHORS
%{_libdir}/ntrack/modules/ntrack-libnl1.so
%{_libdir}/ntrack/modules/ntrack-libnl3_x.so


%files -n %{libntrack}
%{_libdir}/libntrack.so.%{major}*

%files -n %{libntrack_devel}
%{_libdir}/pkgconfig/libntrack.pc
%{_libdir}/libntrack.so

%files -n %{libntrackqt}
%{_libdir}/libntrack-qt4.so.%{ntrackqt_major}*

%files -n %{libntrackqt_devel}
%{_includedir}/%{name}/qt4/
%{_libdir}/pkgconfig/libntrack-qt4.pc
%{_libdir}/libntrack-qt4.so

%files -n %{libntrack_gobject}
%{_libdir}/libntrack-gobject.so.%{libntrack_gobject_major}*

%files -n %{libntrack_gobject_devel}
%{_includedir}/%{name}/gobject/
%{_libdir}/pkgconfig/libntrack-gobject.pc
%{_libdir}/libntrack-gobject.so 

%files -n %{libntrack_glib}
%{_libdir}/libntrack-glib.so.%{libntrack_glib_major}*

%files -n %{libntrack_glib_devel}
%{_includedir}/%{name}/glib/
%{_libdir}/pkgconfig/libntrack-glib.pc
%{_libdir}/libntrack-glib.so

%files devel
%dir %{_includedir}/ntrack
%{_includedir}/ntrack/common/

%files -n python-%{name}
%{_libdir}/python*/site-packages/pyntrack.so


%changelog
* Tue Feb 21 2012 Jon Dill <dillj@mandriva.org> 016-3mdv2012.0
+ Revision: 778765
- rebuild against new version of libffi4

  + Oden Eriksson <oeriksson@mandriva.com>
    - relink against libpcre.so.1

* Tue Nov 22 2011 Oden Eriksson <oeriksson@mandriva.com> 016-1
+ Revision: 732374
- P0: remove -Werror and -pedantic (thanks pcpa)
- enable libnl 3.2.x support
- fix build
- 016

* Fri Sep 02 2011 Oden Eriksson <oeriksson@mandriva.com> 014-4
+ Revision: 697779
- import ntrack


* Fri Aug 26 2011 Oden Eeriksson <oeriksson@mandriva.com> 014-4
- added P1 by pterjan

* Fri Jul 01 2011 mikala <mikala> 014-3.mga2
+ Revision: 117124
- Add patch0 to fix a symbol error (should fix mga #1976)

* Sun Jun 26 2011 mikala <mikala> 014-2.mga2
+ Revision: 113911
- Fix some provides & requires

* Sun Jun 26 2011 mikala <mikala> 014-1.mga2
+ Revision: 113868
- imported package ntrack

