%define major 0
%define libntrack %mklibname ntrack %{major}
%define devntrack %mklibname ntrack -d

%define ntrackqt_major 1
%define libntrackqt %mklibname ntrack-qt4 %ntrackqt_major
%define devntrackqt %mklibname ntrack-qt4 -d 

%define libntrackgobject_major 1
%define libntrackgobject %mklibname ntrack-gobject %{libntrackgobject_major}
%define devntrackgobject %mklibname ntrack-gobject -d

%define libntrackglib_major 2
%define libntrackglib %mklibname ntrack-glib %{libntrackglib_major}
%define devntrackglib %mklibname ntrack-glib -d

Summary:	Network Connectivity Tracking library for Desktop Applications
Name:		ntrack
Version:	016
Release:	20
Group:		Development/C
License:	LGPLv3
Url:		https://launchpad.net/%{name}
Source0:	http://launchpad.net/%{name}/main/%{version}/+download/%{name}-%{version}.tar.gz
Patch0:		ntrack-016-relaxed_flags.diff
Patch1:		ntrack-016-automake-1.13.patch
Patch2:		ntrack-016-libnl-3.2.patch
BuildRequires:	libtool
BuildRequires:	pkgconfig(libnl-3.0)
BuildRequires:	pkgconfig(python2)
BuildRequires:	pkgconfig(pygobject-2.0)
BuildRequires:	pkgconfig(QtCore)

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

%package -n %{devntrack}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		Development/Other
Provides:	ntrack-devel = %{version}-%{release}

%description -n %{devntrack}
ntrack aims to be a lightweight and easy to use library for application
developers that want to get events on network online status changes such
as online, offline or route changes.

%package -n %{libntrackqt}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		System/Libraries
Obsoletes:	%{_lib}ntrack-qt41

%description -n %{libntrackqt}
This packages provides the Qt4 bindings for %{name}.

%package -n %{devntrackqt}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		Development/Other
Requires:	%{libntrackqt} = %{version}-%{release}
Provides:	ntrack-qt4-devel = %{version}-%{release}

%description -n %{devntrackqt}
Development Qt4 files (headers and libraries) for ntrack.

%package -n %{libntrackgobject}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		System/Libraries

%description -n %{libntrackgobject}
This package provides the gobject bindings for %{name}.

%package -n %{devntrackgobject}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		Development/Other
Requires:	%{libntrackgobject} = %{version}-%{release}
Provides:	ntrack-gobject-devel = %{version}-%{release}

%description -n %{devntrackgobject}
Development GObject files (headers and libraries) for ntrack.

%package -n %{libntrackglib}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		System/Libraries

%description -n %{libntrackglib}
This package provides the glib bindings for %{name}.

%package -n %{devntrackglib}
Summary:	Network Connectivity Tracking library for Desktop Applications
Group:		Development/Other
Requires:	%{libntrackglib} = %{version}-%{release}
Provides:	ntrack-glib-devel = %{version}-%{release}

%description -n %{devntrackglib}
Development files (headers and libraries) for ntrack.

%package -n python-%{name}
Summary:	Python bindings to %{name}
Group:		Development/Python

%description -n python-%{name}
Python bindings to %{name}.

%prep
%setup -q
%apply_patches
mkdir -p m4
autoreconf -fi

%build
export PYTHON=%{__python2}
%configure --disable-static
%make

%install
%makeinstall_std
# dupes
rm -rf %{buildroot}%{_datadir}/doc/ntrack

%files
%doc README NEWS COPYING COPYING.LESSER ChangeLog AUTHORS
#{_libdir}/ntrack/modules/ntrack-libnl1.so
%{_libdir}/ntrack/modules/ntrack-libnl3_x.so

%files -n %{libntrack}
%{_libdir}/libntrack.so.%{major}*

%files -n %{devntrack}
%dir %{_includedir}/ntrack
%{_includedir}/ntrack/common/
%{_libdir}/pkgconfig/libntrack.pc
%{_libdir}/libntrack.so

%files -n %{libntrackqt}
%{_libdir}/libntrack-qt4.so.%{ntrackqt_major}*

%files -n %{devntrackqt}
%{_includedir}/%{name}/qt4/
%{_libdir}/pkgconfig/libntrack-qt4.pc
%{_libdir}/libntrack-qt4.so

%files -n %{libntrackgobject}
%{_libdir}/libntrack-gobject.so.%{libntrackgobject_major}*

%files -n %{devntrackgobject}
%{_includedir}/%{name}/gobject/
%{_libdir}/pkgconfig/libntrack-gobject.pc
%{_libdir}/libntrack-gobject.so 

%files -n %{libntrackglib}
%{_libdir}/libntrack-glib.so.%{libntrackglib_major}*

%files -n %{devntrackglib}
%{_includedir}/%{name}/glib/
%{_libdir}/pkgconfig/libntrack-glib.pc
%{_libdir}/libntrack-glib.so

%files -n python-%{name}
%{_libdir}/python*/site-packages/pyntrack.so

