Name:           ntrack
Summary:        Network Connectivity Tracking library for Desktop Applications
Version:        016
Release:        %mkrel 1
License:        LGPLv3
Url:            https://launchpad.net/%{name}
Source:         http://launchpad.net/%{name}/main/%{version}/+download/%{name}-%{version}.tar.gz
Group:          Development/C
BuildRequires:  autoconf automake libtool
BuildRequires:  libqt4-devel
BuildRequires:  libnl-devel
BuildRequires:  python-devel


%description
ntrack aims to be a lightweight and easy to use library for application
developers that want to get events on network online status changes such
as online, offline or route changes.

The primary goal is to serve desktop applications in a network manager 
and desktop environment independent fashion.
Also its supposed to be lightweight, resource un-intensive and extensible.

%files
%defattr(-,root,root)
%doc README NEWS COPYING COPYING.LESSER ChangeLog AUTHORS
%{_libdir}/ntrack/modules/ntrack-libnl1.so 
#------------------------------------------------------------------------------

%define ntrack_major 0
%define libntrack %mklibname ntrack %ntrack_major

%package -n %{libntrack}
Summary:        Network Connectivity Tracking library for Desktop Applications
Group: System/Libraries  

%description -n %{libntrack}
ntrack aims to be a lightweight and easy to use library for application
developers that want to get events on network online status changes such
as online, offline or route changes.
 
%files -n %{libntrack}
%defattr(-,root,root)
%{_libdir}/libntrack.so.%{ntrack_major}*

#-------------------------------------------------------------------------------

%define libntrack_devel %mklibname ntrack -d
%package -n %{libntrack_devel}
Summary:  Network Connectivity Tracking library for Desktop Applications
Group: Development/Other
Provides: libntrack-devel = %{version}-%{release}

%description -n %{libntrack_devel}
ntrack aims to be a lightweight and easy to use library for application
developers that want to get events on network online status changes such
as online, offline or route changes.

%files -n %{libntrack_devel}
%defattr(-,root,root)
%{_libdir}/pkgconfig/libntrack.pc
%{_libdir}/libntrack.so

#-------------------------------------------------------------------------------

%define ntrackqt_major 1
%define libntrackqt %mklibname ntrack-qt4 %ntrackqt_major

%package -n %{libntrackqt} 
Summary:   Network Connectivity Tracking library for Desktop Applications
Group: System/Libraries  

%description -n %{libntrackqt}
ntrack aims to be a lightweight and easy to use library for application
developers that want to get events on network online status changes such
as online, offline or route changes.
 
This packages provides the Qt4 bindings for %{name}
 
%files -n %{libntrackqt}
%defattr(-,root,root)
%{_libdir}/libntrack-qt4.so.%{ntrackqt_major}*

#--------------------------------------------------------------------------------

%define libntrackqt_devel %mklibname libntrack-qt4 -d 
 
%package -n %{libntrackqt_devel}
Summary:   Network Connectivity Tracking library for Desktop Applications
Group: Development/Other
Requires:  %{libntrackqt} = %{version}-%{release}
Provides:  libntrack-qt4-devel = %{version}-%{release}
 
%description -n %{libntrackqt_devel}
Development files (headers and libraries) for ntrack
 
%files -n %{libntrackqt_devel}
%defattr(-,root,root)
%{_includedir}/%{name}/qt4/
%{_libdir}/pkgconfig/libntrack-qt4.pc
%{_libdir}/libntrack-qt4.so

 
#------------------------------------------------------------------------------
 
%define libntrack_gobject_major 1
%define libntrack_gobject %mklibname libntrack-gobject %{libntrack_gobject_major}

%package -n %{libntrack_gobject}
Summary:   Network Connectivity Tracking library for Desktop Applications
Group: System/Libraries  

%description -n %{libntrack_gobject}
ntrack aims to be a lightweight and easy to use library for application
developers that want to get events on network online status changes such
as online, offline or route changes.

This package provides the gobject bindings for %{name}

%files -n %{libntrack_gobject}
%defattr(-,root,root)
%{_libdir}/libntrack-gobject.so.%{libntrack_gobject_major}*


#------------------------------------------------------------------------------ 

%define libntrack_gobject_devel %mklibname libntrack-gobject -d

%package -n %{libntrack_gobject_devel}
Summary:   Network Connectivity Tracking library for Desktop Applications
Group: Development/Other
Requires:  %{libntrack_gobject} = %{version}-%{release}
Provides:  libntrack-gobject-devel = %{version}-%{release}


%description -n %{libntrack_gobject_devel}
Development files (headers and libraries) for ntrack
 

%files -n %{libntrack_gobject_devel}
%defattr(-,root,root)
%{_includedir}/%{name}/gobject/
%{_libdir}/pkgconfig/libntrack-gobject.pc
%{_libdir}/libntrack-gobject.so 

#------------------------------------------------------------------------------ 

%define libntrack_glib_major 2
%define libntrack_glib %mklibname libntrack-glib %{libntrack_glib_major}

%package -n %{libntrack_glib}
Summary:   Network Connectivity Tracking library for Desktop Applications
Group: System/Libraries  

%description -n %{libntrack_glib}
ntrack aims to be a lightweight and easy to use library for application
developers that want to get events on network online status changes such
as online, offline or route changes.

This package provides the glib bindings for %{name}

%files -n %{libntrack_glib}
%defattr(-,root,root)
%{_libdir}/libntrack-glib.so.%{libntrack_glib_major}* 

#------------------------------------------------------------------------------  
%define libntrack_glib_devel %mklibname libntrack-glib -d
 
%package -n %{libntrack_glib_devel} 
Summary:   Network Connectivity Tracking library for Desktop Applications
Group: Development/Other  
Requires:  %{libntrack_glib} = %{version}-%{release}
Provides: libntrack-glib-devel = %{version}-%{release}

%description -n %{libntrack_glib_devel} 
Development files (headers and libraries) for ntrack
 
%files -n %{libntrack_glib_devel} 
%defattr(-,root,root)
%{_includedir}/%{name}/glib/
%{_libdir}/pkgconfig/libntrack-glib.pc
%{_libdir}/libntrack-glib.so
 
#------------------------------------------------------------------------------ 

%package devel
Summary:   Network Connectivity Tracking library for Desktop Applications
Group: Development/Other 
Requires:  %{libntrack_glib_devel} = %{version}-%{release}
Requires:  %{libntrack_gobject} = %{version}-%{release}
Requires:  %{libntrack_devel} = %{version}-%{release}
Requires:  %{libntrackqt_devel} =  %{version}-%{release}

%description devel
Development files (headers and libraries) for ntrack
  
%files devel
%defattr(-,root,root)
%{_datadir}/doc/ntrack/
%dir %{_includedir}/ntrack
%{_includedir}/ntrack/common/


#------------------------------------------------------------------------------
 
%prep
%setup -q 

%build
mkdir -p m4
autoreconf -fi
%configure
%make
 
%install
%makeinstall

# Remove .a & .la files
rm -rf %{buildroot}/%{_libdir}/*.a
rm -rf %{buildroot}/%{_libdir}/*.la
rm -rf %{buildroot}/%{_libdir}/ntrack/modules/*.a
rm -rf %{buildroot}/%{_libdir}/ntrack/modules/*.la

 
%clean
rm -rf %{buildroot}



