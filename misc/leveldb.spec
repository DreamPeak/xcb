Summary:	A fast and lightweight key/value database library by Google
Name:		leveldb
Version:	1.18
Release:	1%{?dist}
Group:		Applications/Databases
URL:		http://code.google.com/p/leveldb/
License:	BSD
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
Source:		%{name}-%{version}.tar.gz
Patch:		%{name}-%{version}-removing-tcmalloc.patch
BuildRequires:	gcc-c++
BuildRequires:	snappy-devel
Requires:	snappy

%description
LevelDB is a fast key-value storage library written at Google that
provides an ordered mapping from string keys to string values.

%package devel
Summary:	Development files for %{name}
Group:		Development/Libraries
Requires:	%{name} = %{version}-%{release}

%description devel
Development files for %{name}.

%prep
%setup -q
%patch -p1

%build
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_libdir}/
cp -Pf lib%{name}.so* %{buildroot}%{_libdir}/
mkdir -p %{buildroot}%{_includedir}/%{name}/
cp -f include/%{name}/*.h %{buildroot}%{_includedir}/%{name}/

%clean
rm -rf %{buildroot}

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(-,root,root,-)
%doc AUTHORS doc/ LICENSE NEWS README TODO
%{_libdir}/lib%{name}.so.*

%files devel
%defattr(-,root,root,-)
%{_includedir}/%{name}/
%{_libdir}/lib%{name}.so

