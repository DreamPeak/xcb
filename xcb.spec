%undefine _missing_build_ids_terminate_build

Name:		xcb
Version:	1.0.2
Release:	5%{?dist}
Summary:	Reliable Multicast Computing
Group:		Applications/Internet
License:	GPLv2
Source0:	%{name}-%{version}.tar.gz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
BuildRequires:	gcc
BuildRequires:	gperftools-devel
BuildRequires:	leveldb-devel
BuildRequires:	openpgm-devel
BuildRequires:	readline-devel
Requires:	gperftools-libs
Requires:	leveldb
Requires:	logrotate
Requires:	openpgm
Requires:	readline

%description
Reliable Multicast Computing.

%package client
Summary:	Client APIs for XCUBE
Group:		Development/Libraries

%description client
Client APIs for XCUBE.

%package devel
Summary:	Development files for XCUBE
Group:		Development/Libraries

%description devel
Development files for XCUBE.

%prep
%setup -q

%build
%configure --disable-static CFLAGS="-march=corei7 -g -O3 -pipe -fno-builtin-memcmp" CXXFLAGS="-march=corei7 -g -O3"
make %{?_smp_mflags}
pushd misc
make
popd

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
install -D -m 0644 %{name}.logrotate        %{buildroot}%{_sysconfdir}/logrotate.d/%{name}
install -D -m 0755 %{name}.init             %{buildroot}%{_initrddir}/%{name}-dp2
sed -i 's|__XCB__|%{name}-dp2|g'            %{buildroot}%{_initrddir}/%{name}-dp2
install -D -m 0755 %{name}.init             %{buildroot}%{_initrddir}/%{name}-wk2
sed -i 's|__XCB__|%{name}-wk2|g'            %{buildroot}%{_initrddir}/%{name}-wk2
install -D -m 0755 %{name}.init             %{buildroot}%{_initrddir}/%{name}-pb2
sed -i 's|__XCB__|%{name}-pb2|g'            %{buildroot}%{_initrddir}/%{name}-pb2
install -D -m 0755 %{name}.init             %{buildroot}%{_initrddir}/%{name}-compact
sed -i 's|__XCB__|%{name}-compact|g'        %{buildroot}%{_initrddir}/%{name}-compact
chmod +x                                    %{buildroot}%{_libdir}/*.so
rm -rf %{buildroot}%{_libdir}/*.la
mkdir -p %{buildroot}%{_sharedstatedir}/%{name}/
mv -f %{buildroot}%{_libdir}/md_*.so  %{buildroot}%{_sharedstatedir}/%{name}/
mv -f %{buildroot}%{_libdir}/app_*.so %{buildroot}%{_sharedstatedir}/%{name}/
mv -f misc/app_ema.so                 %{buildroot}%{_sharedstatedir}/%{name}/

%clean
rm -rf %{buildroot}

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(-,root,root,-)
%exclude %{_libdir}/libxcbclient.so*
%doc AUTHORS ChangeLog COPYING INSTALL NEWS README TODO
%config(noreplace) %{_sysconfdir}/%{name}/*
%config(noreplace) %{_sysconfdir}/logrotate.d/%{name}
%{_initrddir}/*
%{_libdir}/*
%{_sbindir}/*
%{_sharedstatedir}/%{name}/*

%files client
%defattr(-,root,root,-)
%{_includedir}/%{name}/xcb-client.h
%{_libdir}/libxcbclient.so*

%files devel
%defattr(-,root,root,-)
%exclude %{_includedir}/%{name}/xcb-client.h
%{_includedir}/%{name}/*
%{_datadir}/%{name}/*

%changelog

