Name:       context-provider
Summary:    Context Provider
Version:    0.8.0
Release:    1
Group:      Service/Context
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

%define BUILD_PROFILE %{?profile}%{!?profile:%{?tizen_profile_name}}

%if "%{?BUILD_PROFILE}" == "tv"
ExcludeArch: %{arm} aarch64 %ix86 x86_64
%endif

BuildRequires: cmake

BuildRequires: pkgconfig(gmodule-2.0)
BuildRequires: pkgconfig(libcontext-server)
BuildRequires: pkgconfig(vconf)
BuildRequires: pkgconfig(capi-system-info)
BuildRequires: pkgconfig(capi-system-device)
BuildRequires: pkgconfig(capi-system-runtime-info)
BuildRequires: pkgconfig(capi-appfw-package-manager)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-appfw-app-manager)
BuildRequires: pkgconfig(pkgmgr)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(capi-media-sound-manager)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires: pkgconfig(motion)

%if "%{?BUILD_PROFILE}" == "mobile"
BuildRequires: pkgconfig(msg-service)
BuildRequires: pkgconfig(contacts-service2)
BuildRequires: pkgconfig(tapi)
BuildRequires: pkgconfig(capi-telephony)
BuildRequires: pkgconfig(capi-messaging-email)
BuildRequires: pkgconfig(capi-content-media-content)
BuildRequires: pkgconfig(libmedia-utils)
BuildRequires: pkgconfig(capi-location-manager)
BuildRequires: pkgconfig(capi-geofence-manager)
%endif

%description
Context Provider

%prep
%setup -q

%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`

export   CFLAGS+=" -Wextra -Wcast-align -Wshadow -Wwrite-strings -Wswitch-default -Wno-unused-parameter"
export CXXFLAGS+=" -Wextra -Wcast-align -Wshadow -Wwrite-strings -Wswitch-default -Wno-unused-parameter"

export   CFLAGS+=" -Wno-empty-body -fomit-frame-pointer -fno-optimize-sibling-calls"
export CXXFLAGS+=" -Wno-empty-body -fomit-frame-pointer -fno-optimize-sibling-calls"

export   CFLAGS+=" -fno-strict-aliasing -fno-unroll-loops -fsigned-char -fstrict-overflow"
export CXXFLAGS+=" -fno-strict-aliasing -fno-unroll-loops -fsigned-char -fstrict-overflow"

export   CFLAGS+=" -fno-common"
export CXXFLAGS+=" -Wnon-virtual-dtor"
export CXXFLAGS+=" -std=c++11 -Wno-c++11-compat"

#export   CFLAGS+=" -Wcast-qual"
#export CXXFLAGS+=" -Wcast-qual"

#export   CFLAGS+=" -DTIZEN_ENGINEER_MODE"
#export CXXFLAGS+=" -DTIZEN_ENGINEER_MODE"
#export   FFLAGS+=" -DTIZEN_ENGINEER_MODE"

cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DMAJORVER=${MAJORVER} -DFULLVER=%{version} -DPROFILE=%{?BUILD_PROFILE}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%manifest packaging/%{name}.manifest
%defattr(-,root,root,-)
/usr/share/license/%{name}
%{_libdir}/*.so*
%{_libdir}/context-service/*.so*

%package devel
Summary:    Context Provider Shared Header (Development)
Group:      Service/Context
Requires:	%{name} = %{version}-%{release}

%description devel
Context Provider Shared Header (DEV)

%files devel
%defattr(-,root,root,-)
%{_includedir}/context-service/internal/*.h
